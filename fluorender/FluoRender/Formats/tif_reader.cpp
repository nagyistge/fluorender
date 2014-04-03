#include "tif_reader.h"
#include "../compatibility.h"

TIFReader::TIFReader()
{
   m_resize_type = 0;
   m_resample_type = 0;
   m_alignment = 0;

   m_slice_seq = false;
   m_time_num = 0;
   m_cur_time = -1;
   m_chan_num = 0;
   m_slice_num = 0;
   m_x_size = 0;
   m_y_size = 0;

   m_valid_spc = false;
   m_xspc = 1.0;
   m_yspc = 1.0;
   m_zspc = 1.0;

   m_max_value = 0.0;
   m_scalar_scale = 1.0;

   m_batch = false;
   m_cur_batch = -1;

   m_time_id = L"_T";

   current_page_ = current_offset_ = 0;
   swap_ = false;
}

TIFReader::~TIFReader()
{
   if (tiff_stream.is_open())
      tiff_stream.close();
}

void TIFReader::SetFile(string &file)
{
   if (!file.empty())
   {
      if (!m_path_name.empty())
         m_path_name.clear();
      m_path_name.assign(file.length(), L' ');
      copy(file.begin(), file.end(), m_path_name.begin());
   }
   m_id_string = m_path_name;
}

void TIFReader::SetFile(wstring &file)
{
   m_path_name = file;
   m_id_string = m_path_name;
}

void TIFReader::Preprocess()
{
    int i;

   m_4d_seq.clear();

#ifdef _WIN32
   wchar_t slash = L'\\';
#else
   wchar_t slash = L'/';
#endif
   //separate path and name
   size_t pos = m_path_name.find_last_of(slash);
   if (pos == -1)
      return;
   wstring path = m_path_name.substr(0, pos+1);
   wstring name = m_path_name.substr(pos+1);
   //extract time sequence string
   size_t begin = name.find(m_time_id);
   size_t end = -1;
   size_t id_len = m_time_id.size();
   if (begin != -1)
   {
      wstring t_num;
      size_t k;
      for (k=begin+id_len; k<name.size(); k++)
      {
         wchar_t c = name[k];
         if (iswdigit(c))
            t_num.push_back(c);
         else break;
      }
      if (t_num.size() > 0)
         end = k;
      else
         begin = -1;
   }
   //build 4d sequence
   if (begin == -1)
   {
      TimeDataInfo info;
      SliceInfo sliceinfo;
      sliceinfo.slice = name;  //temporary name
      sliceinfo.slicenumber = 0;
      info.slices.push_back(sliceinfo);
      info.filenumber = 0;
      m_4d_seq.push_back(info);
      m_cur_time = 0;
   }
   else
   {
      //search time sequence files
      std::vector< std::wstring> list;
      FIND_FILES(path,L".tif",list, m_cur_time,name.substr(0,begin + id_len + 1 ));
      for(size_t f = 0; f < list.size(); f++) {
         TimeDataInfo inf;
         std::wstring str = list.at(0);
         std::wstring t_num;
         size_t j;
         for(j = begin+id_len;j<str.size();j++)
         {
            wchar_t c = str[j];
            if (iswdigit(c))
               t_num.push_back(c);
            else break;
         }
         if (t_num.size() > 0)
            inf.filenumber = WSTOI(t_num);
         else
            inf.filenumber = 0;
         SliceInfo sliceinfo;
         sliceinfo.slice = str;
         sliceinfo.slicenumber = 0;
         inf.slices.push_back(sliceinfo);
         m_4d_seq.push_back(inf);
      }
      if (m_4d_seq.size() > 0)
         std::sort(m_4d_seq.begin(), m_4d_seq.end(), TIFReader::tif_sort);

      //build 3d slice sequence
      for (int t=0; t<(int)m_4d_seq.size(); t++)
      {
         wstring slice_str = m_4d_seq[t].slices[0].slice;

         if (m_slice_seq)
         {
            //extract common string in name
            size_t pos2 = slice_str.find_last_of(L'.');
            size_t begin2 = 0;
            size_t end2 = -1;
            for (i=int(pos2)-1; i>=0; i--)
            {
               if (iswdigit(slice_str[i]) && end2==-1)
                  end2 = i;
               if (!iswdigit(slice_str[i]) && end2!=-1)
               {
                  begin2 = i;
                  break;
               }
            }
            if (end2!=-1)
            {
               //search slice sequence
               std::vector<std::wstring> list;
               FIND_FILES(path,L".tif",list,m_cur_time,slice_str.substr(0, begin2+1));
               m_4d_seq[t].type = 1;
               for(size_t f = 0; f < list.size(); f++) {
                  size_t start_idx = begin2+1;
                  size_t end_idx   = list.at(i).find(L".tif");
                  size_t size = end_idx - start_idx;
                  std::wstring fileno = list.at(i).substr(start_idx, size);
                  SliceInfo slice;
                  slice.slice = list.at(f);
                  slice.slicenumber = WSTOI(fileno);
                  m_4d_seq[t].slices.push_back(slice);
               }
               if (m_4d_seq[t].slices.size() > 0)
                  std::sort(m_4d_seq[t].slices.begin(),
                        m_4d_seq[t].slices.end(),
                        TIFReader::tif_slice_sort);
            }
         }
         else
         {
            m_4d_seq[t].type = 0;
            m_4d_seq[t].slices[0].slice = path + slice_str;
            if (m_4d_seq[t].slices[0].slice == m_path_name)
               m_cur_time = t;
         }
      }
   }

   //get time number and channel number
   m_time_num = (int)m_4d_seq.size();
   if (m_4d_seq.size()>0 &&
         m_cur_time>=0 &&
         m_cur_time<(int)m_4d_seq.size() &&
         m_4d_seq[m_cur_time].slices.size()>0)
   {
      wstring tiff_name = m_4d_seq[m_cur_time].slices[0].slice;
      if (tiff_name.size()>0)
      {
         OpenTiff(tiff_name);
         m_chan_num = GetTiffField(kSamplesPerPixelTag,nullptr,0);
         if (m_chan_num == 0 &&
               GetTiffField(kImageWidthTag,nullptr,0) > 0&&
               GetTiffField(kImageLengthTag,nullptr,0) > 0) {
            m_chan_num = 1;
         }
         CloseTiff();
      }
      else m_chan_num = 0;
   }
   else m_chan_num = 0;
}

uint32_t TIFReader::GetTiffField(
      const int in_tag, void * buf, uint32_t size)
{
   if (!tiff_stream.is_open())
      throw std::runtime_error( "TIFF File not open for reading." );
   //go to the current IFD block/page
   tiff_stream.seekg(current_offset_,tiff_stream.beg);
   uint16_t num_entries=0;
   //how many entries are there?
   tiff_stream.read((char*)&num_entries,sizeof(uint16_t));
   if (swap_) num_entries = SwapShort(num_entries);
   //if we simply want the next page offset, get that and return.
   if (in_tag == kNextPageOffsetTag) {
      uint32_t next_offset = 0;
      tiff_stream.seekg(current_offset_+2+12*num_entries,tiff_stream.beg);
      tiff_stream.read((char*)&next_offset,sizeof(uint32_t));
      return swap_?SwapWord(next_offset):next_offset;
   }
   //go through all of the entries to find the one we want
   for(size_t i = 0; i < num_entries; i++) {
      tiff_stream.seekg(current_offset_+2+12*i,tiff_stream.beg);
      uint16_t tag=0;
      //get the tag of entry
      tiff_stream.read((char*)&tag,sizeof(uint16_t));
      if (swap_) tag = SwapShort(tag);
      // This is the correct tag, grab and go.
      if (tag == in_tag) {
         //find the type
         tiff_stream.seekg(current_offset_+2+12*i+2,tiff_stream.beg);
         uint16_t type=0;
         tiff_stream.read((char*)&type,sizeof(uint16_t));
         if (swap_) type = SwapShort(type);
         if (!buf && size == kType) return static_cast<uint32_t>(type);
         //find the count
         tiff_stream.seekg(current_offset_+2+12*i+4,tiff_stream.beg);
         uint32_t cnt = 0;
         tiff_stream.read((char*)&cnt,sizeof(uint32_t));
         if (swap_) cnt = SwapWord(cnt);
         if (!buf && size == kCount) return cnt;
         //now get the value (different for different types.)
         tiff_stream.seekg(current_offset_+2+12*i+8,tiff_stream.beg);
         uint32_t answer=0;
         if (type == kByte) {
            uint8_t value=0;
            tiff_stream.read((char*)&value,sizeof(uint8_t));
            answer = static_cast<uint32_t>(value);
         } else if (type == kASCII) {
            if (cnt > 1) {
               tiff_stream.read((char*)&answer,sizeof(uint32_t));
               answer = swap_?SwapWord(answer):answer;
               tiff_stream.seekg(answer,tiff_stream.beg);
            }
            //read the the string
            tiff_stream.read((char*)buf,min(size,cnt));
         } else if (type == kLong || cnt > 1) {
            uint32_t value=0;
            tiff_stream.read((char*)&value,sizeof(uint32_t));
            if (swap_) value = SwapWord(value);
            answer = value;
            if (in_tag == kBitsPerSampleTag) {
               tiff_stream.seekg(value,tiff_stream.beg);
               uint16_t value1=0;
               tiff_stream.read((char*)&value1,sizeof(uint16_t));
               if (swap_) value1 = SwapShort(value1);
               answer = value1;
            }
         } else if (type == kShort) {
            uint16_t value=0;
            tiff_stream.read((char*)&value,sizeof(uint16_t));
            if (swap_) value = SwapShort(value);
            answer = static_cast<uint32_t>(value);
         } else if (type == kRational) {
            //get the two values in the data to make a float.
            uint32_t value=0;
            tiff_stream.read((char*)&value,sizeof(uint32_t));
            if (swap_) value = SwapWord(value);
            tiff_stream.seekg(value,tiff_stream.beg);
            uint32_t num = 0, den = 0;
            tiff_stream.read((char*)&num,sizeof(uint32_t));
            if (swap_) num = SwapWord(num);
            tiff_stream.seekg(value+sizeof(uint32_t),tiff_stream.beg);
            tiff_stream.read((char*)&den,sizeof(uint32_t));
            if (swap_) den = SwapWord(den);
            float rat = static_cast<float>(num) /
               static_cast<float>(den);
            memcpy(buf,&rat,min((size_t)size,sizeof(float)));
            return 0;
         } else {
            std::cerr << "Unhandled TIFF Tag type" << std::endl;
            exit(0);
         }
         return answer;
      }
   }
   return 0;
}

uint16_t TIFReader::SwapShort(uint16_t num) {
   return ((num & 0x00FF) << 8) | ((num & 0xFF00) >> 8);
}

uint32_t TIFReader::SwapWord(uint32_t num) {
   return ((num & 0x000000FF) << 24) | ((num & 0xFF000000) >> 24) |
      ((num & 0x0000FF00) << 8)  | ((num & 0x00FF0000) >> 8);
}

void TIFReader::SetSliceSeq(bool ss)
{
   //enable searching for slices
   m_slice_seq = ss;
}

bool TIFReader::GetSliceSeq()
{
   return m_slice_seq;
}

void TIFReader::SetTimeId(wstring &id)
{
   m_time_id = id;
}

wstring TIFReader::GetTimeId()
{
   return m_time_id;
}

void TIFReader::SetBatch(bool batch)
{
#ifdef _WIN32
   wchar_t slash = L'\\';
#else
   wchar_t slash = L'/';
#endif
   if (batch)
   {
      //separate path and name
      size_t pos = m_path_name.find_last_of(slash);
      if (pos == -1)
         return;
      wstring path = m_path_name.substr(0, pos+1);
      FIND_FILES(path,L".tif", m_batch_list,m_cur_batch);
      m_batch = true;
   }
   else
      m_batch = false;
}

bool TIFReader::IsNewBatchFile(wstring name)
{
   if (m_batch_list.size() == 0)
      return true;

   for (int i=0; i<(int)m_batch_list.size(); i++)
   {
      if (IsBatchFileIdentical(name, m_batch_list[i]))
         return false;
   }

   return true;
}

bool TIFReader::IsBatchFileIdentical(wstring name1, wstring name2)
{
   if (m_4d_seq.size() > 1)
   {
      size_t pos = name1.find(m_time_id);
      if (pos == -1)
         return false;
      wstring find_str = name1.substr(0, pos+2);
      pos = name2.find(find_str);
      if (pos == -1)
         return false;
      else
         return true;
   }
   else if (m_slice_seq)
   {
      size_t pos = name1.find_last_of(L'.');
      size_t begin = -1;
      size_t end = -1;
      for (int i=int(pos)-1; i>=0; i--)
      {
         if (iswdigit(name1[i]) && end==-1 && begin==-1)
            end = i;
         if (!iswdigit(name1[i]) && end!=-1 && begin==-1)
         {
            begin = i;
            break;
         }
      }
      if (begin == -1)
         return false;
      else
      {
         wstring find_str = name1.substr(0, begin+1);
         pos = name2.find(find_str);
         if (pos == -1)
            return false;
         else
            return true;
      }
   }
   else
   {
      if (name1 == name2)
         return true;
      else
         return false;
   }
}

int TIFReader::LoadBatch(int index)
{
   int result = -1;
   if (index>=0 && index<(int)m_batch_list.size())
   {
      m_path_name = m_batch_list[index];
      Preprocess();
      result = index;
      m_cur_batch = result;
   }
   else
      result = -1;

   return result;
}

int TIFReader::LoadOffset(int offset)
{
   int result = m_cur_batch + offset;

   if (offset > 0)
   {
      if (result<(int)m_batch_list.size())
      {
         m_path_name = m_batch_list[result];
         Preprocess();
         m_cur_batch = result;
      }
      else if (m_cur_batch<(int)m_batch_list.size()-1)
      {
         result = (int)m_batch_list.size()-1;
         m_path_name = m_batch_list[result];
         Preprocess();
         m_cur_batch = result;
      }
      else
         result = -1;
   }
   else if (offset < 0)
   {
      if (result >= 0)
      {
         m_path_name = m_batch_list[result];
         Preprocess();
         m_cur_batch = result;
      }
      else if (m_cur_batch > 0)
      {
         result = 0;
         m_path_name = m_batch_list[result];
         Preprocess();
         m_cur_batch = result;
      }
      else
         result = -1;
   }
   else
      result = -1;

   return result;
}

Nrrd* TIFReader::Convert(int t, int c, bool get_max)
{
   if (t<0 || t>=m_time_num ||
         c<0 || c>=m_chan_num)
      return 0;

#ifdef _WIN32
   wchar_t slash = L'\\';
#else
   wchar_t slash = L'/';
#endif
   Nrrd* data = 0;
   TimeDataInfo chan_info = m_4d_seq[t];
   m_data_name = chan_info.slices[0].slice.substr(
         chan_info.slices[0].slice.find_last_of(slash)+1);
   data = ReadTiff(chan_info.slices, c, get_max);
   m_cur_time = t;
   return data;
}

wstring TIFReader::GetCurName(int t, int c)
{
   if (t>=0 && t<m_4d_seq.size())
      return (m_4d_seq[t].slices)[0].slice;
   else
      return L"";
}

bool TIFReader::tif_sort(const TimeDataInfo& info1, const TimeDataInfo& info2)
{
   return info1.filenumber < info2.filenumber;
}

bool TIFReader::tif_slice_sort(const SliceInfo& info1, const SliceInfo& info2)
{
   return info1.slicenumber < info2.slicenumber;
}

uint32_t TIFReader::GetNumTiffPages()
{
   uint32_t count = 0;
   uint32_t save_offset = current_offset_;
   uint32_t save_page = current_page_;
   ResetTiff();
   while (true) {
      // count it if it's not a thumbnail
      if (GetTiffField(kSubFileTypeTag,nullptr,0) != 1) count++;
      current_offset_ = GetTiffField(kNextPageOffsetTag,nullptr,0);
      if (current_offset_ == 0) break;
   }
   current_offset_ = save_offset;
   current_page_ = save_page;
   return count;
}

void TIFReader::GetTiffStrip(uint32_t page, uint32_t strip,
      void * data, uint32_t strip_size)
{
   //make sure we are on the correct page,
   //reset if ahead
   if (current_page_ > page)
      ResetTiff();
   // fast forward if we are behind.
   while (current_page_ < page) {
      tiff_stream.seekg(current_offset_,tiff_stream.beg);
      if (GetTiffField(kSubFileTypeTag,nullptr,0) != 1) current_page_ ++;
      current_offset_ = GetTiffField(kNextPageOffsetTag,nullptr,0);
   }
   //get the byte count and the strip offset to read data from.
   uint32_t byte_count = GetTiffStripOffsetOrCount(kStripBytesCountTag,strip);
   tiff_stream.seekg(GetTiffStripOffsetOrCount(
            kStripOffsetsTag,strip),tiff_stream.beg);
   //actually read the data now
   char *temp = new char[byte_count];
   tiff_stream.read((char*)temp,byte_count);
   //get compression tag, decompress if necessary
   uint32_t tmp = GetTiffField(kCompressionTag,nullptr,0);
   bool isCompressed = tmp == 5;
   if (isCompressed)
      LZWDecode((tidata_t)temp, (tidata_t)data, strip_size);
   else
      memcpy(data,temp,byte_count);
   delete[] temp;
}

uint32_t TIFReader::GetTiffStripOffsetOrCount(uint32_t tag, uint32_t strip)
{
   //search for the offset that tells us the byte count for this strip
   bool isLong = GetTiffField(tag,nullptr,kType) == kLong;
   uint32_t offset = GetTiffField(tag, nullptr,kValue);
   if(GetTiffField(tag, nullptr, kCount) > 1) {
      offset += strip * (isLong?4:2);
      tiff_stream.seekg(offset, tiff_stream.beg);
   }
   else return offset;
   uint32_t value = 0;
   if (isLong) {
      uint32_t tmp = 0;
      tiff_stream.read((char*)&tmp,sizeof(uint32_t));
      value = (swap_)?SwapWord(tmp):tmp;
   } else {
      uint16_t tmp = 0;
      tiff_stream.read((char*)&tmp,sizeof(uint16_t));
      value = (swap_)?SwapShort(tmp):tmp;
   }
   return value;
}

void TIFReader::ResetTiff()
{
   if (!tiff_stream.is_open())
      throw std::runtime_error( "TIFF file not open for reading." );
   //find the first IFD block/page
   tiff_stream.seekg(4,tiff_stream.beg);
   tiff_stream.read((char*)&current_offset_,sizeof(uint32_t));
   if (swap_) current_offset_ = SwapWord(current_offset_);
   current_page_ = 0;
}

void TIFReader::OpenTiff(std::wstring name)
{
   //open the stream
   tiff_stream.open(ws2s(name), std::ifstream::binary);
   if (!tiff_stream.is_open())
      throw std::runtime_error( "Unable to open TIFF File for reading." );
   tiff_stream.seekg(2,tiff_stream.beg);
   uint16_t tiff_num=0;
   tiff_stream.read((char*)&tiff_num,sizeof(uint16_t));
   swap_ =  SwapShort(tiff_num) == 42;
   // make sure this is a proper tiff and set the state.
   if (tiff_num == 42 || swap_) {
      ResetTiff();
   } else {
      throw std::runtime_error( "TIFF file formatted incorrectly. Wrong Type." );
   }
}

void TIFReader::CloseTiff() { if (tiff_stream.is_open()) tiff_stream.close(); }

Nrrd* TIFReader::ReadTiff(std::vector<SliceInfo> &filelist,
      int c, bool get_max) {
   uint32_t numPages = static_cast<uint32_t>(filelist.size());
   if (numPages <= 0)
      return 0;
   wstring filename = filelist[0].slice;
   OpenTiff(filename.c_str());
   bool sequence = numPages > 1;
   if (!sequence) {
      if (get_max)
         numPages = GetNumTiffPages();
      else
         numPages = m_slice_num;
   }

   uint32_t width = GetTiffField(kImageWidthTag,nullptr,0);
   uint32_t height = GetTiffField(kImageLengthTag,nullptr,0);
   uint16 bits = GetTiffField(kBitsPerSampleTag,nullptr,0);
   uint16 samples = GetTiffField(kSamplesPerPixelTag,nullptr,0);
   if (samples == 0 && width > 0 && height > 0) samples = 1;

   float x_res = 0.0, y_res = 0.0, z_res = 0.0;
   GetTiffField(kXResolutionTag,&x_res,sizeof(float));
   GetTiffField(kYResolutionTag,&y_res,sizeof(float));
   uint32_t rowsperstrip = GetTiffField(kRowsPerStripTag,nullptr,0);
   uint32_t strip_size = rowsperstrip * width * samples * (bits/8);

   char img_desc[256];
   GetTiffField(kImageDescriptionTag, img_desc, 256);
   if (img_desc){
      string desc = string ((char*)img_desc);
      size_t start = desc.find("spacing=");
      if (start!=-1) {
         string spacing = desc.substr(start+8);
         size_t end = spacing.find("\n");
         if (end != -1)
            z_res = static_cast<float>(
                  atof(spacing.substr(0, end).c_str()));
      }
   }

   if (x_res>0.0 && y_res>0.0 && z_res>0.0) {
      m_xspc = 1.0/x_res;
      m_yspc = 1.0/y_res;
      m_zspc = z_res;
      m_valid_spc = true;
   } else {
      m_valid_spc = false;
      m_xspc = 1.0;
      m_yspc = 1.0;
      m_zspc = 1.0;
   }

   if (m_resize_type == 1 && m_alignment > 1) {
      m_x_size = (width/m_alignment+(width%m_alignment?1:0))*m_alignment;
      m_y_size = (height/m_alignment+(height%m_alignment?1:0))*m_alignment;
   } else {
      m_x_size = width;
      m_y_size = height;
   }

   m_slice_num = numPages;
   int pagepixels = m_x_size*m_y_size;

   if (sequence) CloseTiff();

   Nrrd *nrrdout = nrrdNew();

   //allocate memory
   void *val = 0;
   bool eight_bit = bits == 8;

   val = malloc((long long)m_x_size * (long long)m_y_size *
         (long long)numPages * (eight_bit?1:2));
   if (!val)
      throw std::runtime_error( "Unable to allocate memory to read TIFF." );

   int max_value = 0;

   void* buf = 0;
   if (samples > 1)
      buf = malloc(strip_size);

   for (uint32_t pageindex=0; pageindex < numPages; pageindex++)
   {
      if (sequence) {
         filename = filelist[pageindex].slice;
         OpenTiff(filename);
      }
      //this is a thumbnail, skip
      if (GetTiffField(kSubFileTypeTag,nullptr,0) == 1) {
         if (sequence) CloseTiff();
         continue;
      }

      uint32_t num_strips = GetTiffField(kStripOffsetsTag, nullptr,kCount);

      //read file
      for (uint32_t strip=0; strip<num_strips; strip++)
      {
         long long valindex;
         int indexinpage;
         if (samples > 1) {
            GetTiffStrip(sequence?0:pageindex, strip, buf,strip_size);
            int num_pixels = strip_size/samples/(eight_bit?1:2);
            indexinpage = strip*num_pixels;
            valindex = pageindex*pagepixels + indexinpage;
            for (int i=0; i<num_pixels; i++) {
               if (indexinpage++ >= pagepixels) break;
               if (eight_bit)
                  memcpy((uint8_t*)val+valindex,
                        (uint8_t*)buf+samples*i+c,sizeof(uint8_t));
               else
                  memcpy((uint16_t*)val+valindex*2,
                        (uint16_t*)buf+samples*i+c,sizeof(uint16_t));
               if (!eight_bit && get_max &&
                     *((uint16_t*)val+valindex) > max_value)
                  max_value = *((uint16_t*)val+valindex);
               if(eight_bit) valindex ++;
            }
         } else {
            valindex = pageindex*pagepixels +
               strip*strip_size/(eight_bit?1:2);
            if (eight_bit)
               GetTiffStrip(sequence?0:pageindex, strip,
                     (uint8_t*)val+valindex,strip_size);
            else
               GetTiffStrip(sequence?0:pageindex, strip,
                     (uint16_t*)val+valindex,strip_size);
         }
      }
      if (sequence) CloseTiff();
   }

   if (samples > 1 && buf)
      free(buf);
   if (!sequence) CloseTiff();

   //write to nrrd
   if (eight_bit)
      nrrdWrap(nrrdout, (uint8_t*)val, nrrdTypeUChar,
            3, (size_t)m_x_size, (size_t)m_y_size, (size_t)numPages);
   else
      nrrdWrap(nrrdout, (uint16_t*)val, nrrdTypeUShort,
            3, (size_t)m_x_size, (size_t)m_y_size, (size_t)numPages);
   nrrdAxisInfoSet(nrrdout, nrrdAxisInfoSpacing, m_xspc, m_yspc, m_zspc);
   nrrdAxisInfoSet(nrrdout, nrrdAxisInfoMax, m_xspc*m_x_size,
         m_yspc*m_y_size, m_zspc*numPages);
   nrrdAxisInfoSet(nrrdout, nrrdAxisInfoMin, 0.0, 0.0, 0.0);
   nrrdAxisInfoSet(nrrdout, nrrdAxisInfoSize, (size_t)m_x_size,
         (size_t)m_y_size, (size_t)numPages);

   if (!eight_bit) {
      if (get_max) {
         if (samples > 1)
            m_max_value = max_value;
         else {
            double value;
            for (int i=0; i<m_slice_num*m_x_size*m_y_size; i++) {
               value= ((unsigned short*)nrrdout->data)[i];
               m_max_value = value>m_max_value ? value : m_max_value;
            }
         }
      }
      if (m_max_value > 0.0) m_scalar_scale = 65535.0 / m_max_value;
      else m_scalar_scale = 1.0;
   } else m_max_value = 255.0;

   return nrrdout;
}

