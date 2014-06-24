#include <wx/wx.h>

#ifndef _SETTINGDLG_H_
#define _SETTINGDLG_H_

#define SETTING_FILE_NAME "fluorender.set"

//enum BitmapFontType;
class SettingDlg : public wxPanel
{
	enum
	{
		//project save
		ID_PrjSaveChk = wxID_HIGHEST+1201,
		//real time compress
		ID_RealtimeCmpChk,
		//mouse interactions
		ID_MouseIntChk,
		//depth peeling
		ID_PeelingLayersSldr,
		ID_PeelingLayersText,
		//micro blending
		ID_MicroBlendChk,
		//shadow
		ID_ShadowDirChk,
		ID_ShadowDirSldr,
		ID_ShadowDirText,
		//gradient background
		ID_GradBgChk,
		//override vox
		ID_OverrideVoxChk,
		//wavelength to color
		ID_WavColor1Cmb,
		ID_WavColor2Cmb,
		ID_WavColor3Cmb,
		ID_WavColor4Cmb,
		//memory settings
		ID_StreamingChk,
		ID_GraphicsMemSldr,
		ID_GraphicsMemText,
		ID_LargeDataSldr,
		ID_LargeDataText,
		ID_BlockSizeSldr,
		ID_BlockSizeText,
		ID_ResponseTimeSldr,
		ID_ResponseTimeText,
		//font
		ID_FontCmb,
		//script
		ID_RunScriptChk,
		//save/close
		ID_SaveBtn,
		ID_CloseBtn
	};

public:
	SettingDlg(wxWindow* frame,
		wxWindow* parent);
	~SettingDlg();

	void GetSettings();
	void UpdateUI();
	void SaveSettings();

	//get settings from ui
	int GetGMCMode() {return m_gmc_mode;}
	bool GetProjSave() {return m_prj_save;}
	bool GetRealtimeCompress() {return m_realtime_compress;}
	void SetRealtimeCompress(bool val) {m_realtime_compress = val;}
	bool GetTestMode(int type);	//type	1:speed test
								//		2:parameter test
								//		3:wireframe mode
	int GetPeelingLyers();
	bool GetMicroBlend();
	void GetShadowDir(double& x, double &y);
	bool GetMouseInt() {return m_mouse_int;}
	int GetWavelengthColor(int n);
	wxString GetTimeId() {return m_time_id;}
	void SetTimeId(wxString id) {m_time_id = id;}
	//gradient background
	bool GetGradBg() {return m_grad_bg;}
	void SetGradBg(bool val) {m_grad_bg = val;}
	//override vox
	bool GetOverrideVox() {return m_override_vox;}
	void SetOverrideVox(bool val) {m_override_vox = val;}
	//soft threshold
	double GetSoftThreshold() {return m_soft_threshold;}
	void SetSoftThreshold(double val) {m_soft_threshold = val;}
	//run script
	bool GetRunScript() {return m_run_script;}
	void SetRunScript(bool val) {m_run_script = val;}
	//text font
	int GetTextFont() {return m_text_font;}
	void SetTextFont(int font) {m_text_font = font;}
	//memory settings
	bool GetMemSwap() {return m_mem_swap;}
	void SetMemSwap(bool val) {m_mem_swap = val;}
	double GetGraphicsMem() {return m_graphics_mem;}
	void SetGraphicsMem(double val) {m_graphics_mem = val;}
	double GetLargeDataSize() {return m_large_data_size;}
	void SetLargeDataSize(double val) {m_large_data_size = val;}
	int GetForceBrickSize() {return m_force_brick_size;}
	void SetForceBrickSize(int val) {m_force_brick_size = val;}
	int GetResponseTime() {return m_up_time;}
	void SetResponseTime(int val) {m_up_time = val;}
	int GetUpdateOrder() {return m_update_order;}
	void SetUpdateOrder(int val) {m_update_order = val;}
	//point volume mode
	int GetPointVolumeMode() {return m_point_volume_mode;}
	void SetPointVolumeMode(int mode) {m_point_volume_mode = mode;}
	//ruler use transfer function
	bool GetRulerUseTransf() {return m_ruler_use_transf;}
	void SetRulerUseTransf(bool val) {m_ruler_use_transf = val;}
	//ruler time dependent
	bool GetRulerTimeDep() {return m_ruler_time_dep;}
	void SetRulerTimeDep(bool val) {m_ruler_time_dep = val;}

private:
	wxWindow* m_frame;

	int m_gmc_mode;			//1-pre-calculated (removed);
							//2-real-time 7 sample;
							//3-real-time 4 samples (removed);
							//4-pre-calculated 4 samples (removed);
	bool m_prj_save;		//save project automatically
	bool m_realtime_compress;//real time compress
	bool m_test_speed;		//test fps
	bool m_test_param;		//using parameter test window
	bool m_test_wiref;		//draw wireframe of volumes
	int m_peeling_layers;	//peeling layer number
	bool m_micro_blend;		//blending slice in depth mode
	bool m_shadow_dir;		//enable directional shaow
	double m_shadow_dir_x;	//x comp of shadow direction
	double m_shadow_dir_y;	//y comp of shadow direction
	bool m_mouse_int;		//enable lower sample rate for mouse interactions
	int m_wav_color1;		//wavelength to color
	int m_wav_color2;		//1-red; 2-green; 3-blue; 4-purple; 5-white
	int m_wav_color3;
	int m_wav_color4;
	wxString m_time_id;		//identfier for time sequence
	bool m_grad_bg;
	bool m_override_vox;
	double m_soft_threshold;
	bool m_run_script;
	//text size
	int m_text_font;		//text size in viewport
	//memory limit
	bool m_mem_swap;		//enable memory swap
	double m_graphics_mem;	//in MB
							//it's the user setting
							//final value is determined by both reading from the card and this value
	double m_large_data_size;//data size considered as large and needs forced bricking
	int m_force_brick_size;	//in pixels
							//it's the user setting
							//final value is determined by both reading from the card and this value
	int m_up_time;			//response time in ms
	int m_update_order;		//0:back-to-front; 1:front-to-back
	//point volume mode
	int m_point_volume_mode;
	//ruler use transfer function
	bool m_ruler_use_transf;
	//ruler time dependent
	bool m_ruler_time_dep;

private:
	//save project
	wxCheckBox *m_prj_save_chk;
	//realtime compress
	wxCheckBox *m_realtime_cmp_chk;
	//mouse interations
	wxCheckBox *m_mouse_int_chk;
	//depth peeling
	wxSlider *m_peeling_layers_sldr;
	wxTextCtrl *m_peeling_layers_text;
	//micro blending
	wxCheckBox *m_micro_blend_chk;
	//shaodw direction
	wxCheckBox *m_shadow_dir_chk;
	wxSlider *m_shadow_dir_sldr;
	wxTextCtrl *m_shadow_dir_text;
	//gradient background
	wxCheckBox *m_grad_bg_chk;
	//override vox
	wxCheckBox *m_override_vox_chk;
	//wavelength to color
	wxComboBox *m_wav_color1_cmb;
	wxComboBox *m_wav_color2_cmb;
	wxComboBox *m_wav_color3_cmb;
	wxComboBox *m_wav_color4_cmb;
	//memory settings
	wxCheckBox *m_streaming_chk;
	wxSlider *m_graphics_mem_sldr;
	wxTextCtrl *m_graphics_mem_text;
	wxSlider *m_large_data_sldr;
	wxTextCtrl *m_large_data_text;
	wxSlider *m_block_size_sldr;
	wxTextCtrl *m_block_size_text;
	wxSlider *m_response_time_sldr;
	wxTextCtrl *m_response_time_text;
	//font
	wxComboBox *m_font_cmb;
	//script
	wxCheckBox *m_run_script_chk;

	//save
	wxButton *m_save_btn;
	//close
	wxButton *m_close_btn;

	//streaming disable/enable
	void EnableStreaming(bool enable);

	//shadow direction
	void SetShadowDir(double deg);
	double GetShadowDir();

	//event handling
	void OnSave(wxCommandEvent &event);
	void OnClose(wxCommandEvent &event);
	void OnShow(wxShowEvent &event);

	wxWindow* CreateProjectPage(wxWindow *parent);
	wxWindow* CreatePerformancePage(wxWindow *parent);
	wxWindow* CreateRenderingPage(wxWindow *parent);
	wxWindow* CreateFormatPage(wxWindow *parent);

	//projec save
	void OnProjectSaveCheck(wxCommandEvent &event);
	void OnRealtimeCompressCheck(wxCommandEvent &event);
	//mouse interaction
	void OnMouseIntCheck(wxCommandEvent &event);
	//peeling
	void OnPeelingLayersChange(wxScrollEvent &event);
	void OnPeelingLayersEdit(wxCommandEvent &event);
	//blending
	void OnMicroBlendCheck(wxCommandEvent &event);
	//shadow
	void OnShadowDirCheck(wxCommandEvent &event);
	void OnShadowDirChange(wxScrollEvent &event);
	void OnShadowDirEdit(wxCommandEvent &event);
	//gradient background
	void OnGradBgCheck(wxCommandEvent &event);
	//override vox
	void OnOverrideVoxCheck(wxCommandEvent &event);
	//wavelength color
	void OnWavColor1Change(wxCommandEvent &event);
	void OnWavColor2Change(wxCommandEvent &event);
	void OnWavColor3Change(wxCommandEvent &event);
	void OnWavColor4Change(wxCommandEvent &event);
	//memory settings
	void OnStreamingChk(wxCommandEvent &event);
	void OnGraphicsMemChange(wxScrollEvent &event);
	void OnGraphicsMemEdit(wxCommandEvent &event);
	void OnLargeDataChange(wxScrollEvent &event);
	void OnLargeDataEdit(wxCommandEvent &event);
	void OnBlockSizeChange(wxScrollEvent &event);
	void OnBlockSizeEdit(wxCommandEvent &event);
	void OnResponseTimeChange(wxScrollEvent &event);
	void OnResponseTimeEdit(wxCommandEvent &event);
	//font
	void OnFontChange(wxCommandEvent &event);
	//script
	void OnRunScriptChk(wxCommandEvent &event);

	DECLARE_EVENT_TABLE();
};

#endif//_SETTINGDLG_H_
