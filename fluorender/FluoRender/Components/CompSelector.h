/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2014 Scientific Computing and Imaging Institute,
University of Utah.


Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
#ifndef FL_CompSelector_h
#define FL_CompSelector_h

#include <vector>
#include <CellList.h>

class VolumeData;
namespace FL
{
	class ComponentSelector
	{
	public:
		ComponentSelector(VolumeData* vd);
		~ComponentSelector();

		void SetVolume(VolumeData* vd)
		{ m_vd = vd; }
		VolumeData* GetVolume()
		{ return m_vd; }
		void SetSelAll(bool value)
		{ m_sel_all = value; }
		bool GetSelAll()
		{ return m_sel_all; }
		void SetId(unsigned int id)
		{ m_id = id; }
		unsigned int GetId()
		{ return m_id; }
		void SetMinNum(bool use, unsigned int num)
		{ m_use_min = use; m_min_num = num; }
		void SetMaxNum(bool use, unsigned int num)
		{ m_use_max = use; m_max_num = num; }

		void CompFull();
		void All();
		void Append(bool all);
		void Exclusive();
		void Clear();
		void Delete();
		void Delete(std::vector<unsigned int> &ids);
		void SelectList(CellList& list);

	private:
		VolumeData* m_vd;
		bool m_sel_all;
		unsigned int m_id;
		bool m_use_min;
		bool m_use_max;
		unsigned int m_min_num;
		unsigned int m_max_num;

	private:
		bool CompareSize(unsigned int size);
	};

	inline bool ComponentSelector::CompareSize(
		unsigned int size)
	{
		if (m_use_min && m_use_max)
		{
			if (size > m_min_num && size < m_max_num)
				return true;
			else
				return false;
		}
		else if (m_use_min)
		{
			if (size > m_min_num)
				return true;
			else
				return false;
		}
		else if (m_use_max)
		{
			if (size < m_max_num)
				return true;
			else
				return false;
		}
		else
			return false;
	}
}
#endif//FL_CompSelector_h