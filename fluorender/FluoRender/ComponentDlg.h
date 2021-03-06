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
#include "Main.h"
#include "DataManager.h"
#include "Components/CompAnalyzer.h"
#include <wx/wx.h>
#include <wx/collpane.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>

#ifndef _COMPONENTDLG_H_
#define _COMPONENTDLG_H_

class VRenderView;
class VolumeData;

DECLARE_APP(VRenderApp)
class ComponentDlg : public wxPanel
{
public:
	enum
	{
		// load / save
		ID_LoadSettingsText = wxID_HIGHEST + 2401,
		ID_LoadSettingsBtn,
		ID_SaveSettingsBtn,
		ID_SaveasSettingsBtn,
		//initial grow pane
		ID_InitialGrowPane,
		ID_InitialGrowCheck,
		ID_IGParamTransitionCheck,
		ID_IGIterationsSldr,
		ID_IGIterationsText,
		//translate
		ID_IGTranslateSldr,
		ID_IGTranslateText,
		ID_IGTranslate2Sldr,
		ID_IGTranslate2Text,
		//scalar falloff
		ID_IGScalarFalloffSldr,
		ID_IGScalarFalloffText,
		ID_IGScalarFalloff2Sldr,
		ID_IGScalarFalloff2Text,
		//grad falloff
		ID_IGGradFalloffSldr,
		ID_IGGradFalloffText,
		ID_IGGradFalloff2Sldr,
		ID_IGGradFalloff2Text,
		//variance falloff
		ID_IGVarFalloffSldr,
		ID_IGVarFalloffText,
		ID_IGVarFalloff2Sldr,
		ID_IGVarFalloff2Text,
		//angle falloff
		ID_IGAngleFalloffSldr,
		ID_IGAngleFalloffText,
		ID_IGAngleFalloff2Sldr,
		ID_IGAngleFalloff2Text,
		
		//sized grow pane
		ID_SizedGrowPane,
		ID_SizedGrowCheck,
		ID_SGParamTransitionCheck,
		ID_SGIterationsSldr,
		ID_SGIterationsText,
		//size limiter
		ID_SGSizeLimiterSldr,
		ID_SGSizeLimiterText,
		ID_SGSizeLimiter2Sldr,
		ID_SGSizeLimiter2Text,
		//translate
		ID_SGTranslateSldr,
		ID_SGTranslateText,
		ID_SGTranslate2Sldr,
		ID_SGTranslate2Text,
		//scalar falloff
		ID_SGScalarFalloffSldr,
		ID_SGScalarFalloffText,
		ID_SGScalarFalloff2Sldr,
		ID_SGScalarFalloff2Text,
		//grad falloff
		ID_SGGradFalloffSldr,
		ID_SGGradFalloffText,
		ID_SGGradFalloff2Sldr,
		ID_SGGradFalloff2Text,
		//variance falloff
		ID_SGVarFalloffSldr,
		ID_SGVarFalloffText,
		ID_SGVarFalloff2Sldr,
		ID_SGVarFalloff2Text,
		//angle falloff
		ID_SGAngleFalloffSldr,
		ID_SGAngleFalloffText,
		ID_SGAngleFalloff2Sldr,
		ID_SGAngleFalloff2Text,

		//cleanup pane
		ID_CleanupPane,
		ID_CleanupCheck,
		ID_CLIterationsSldr,
		ID_CLIterationsText,
		ID_CLSizeLimiterSldr,
		ID_CLSizeLimiterText,

		//match slices pane
		ID_MatchSlicesPane,
		ID_MatchSlicesCheck,
		ID_BidirMatchCheck,
		ID_SizeThreshSldr,
		ID_SizeThreshText,
		ID_SizeRatioSldr,
		ID_SizeRatioText,
		ID_DistThreshSldr,
		ID_DistThreshText,
		ID_AngleThreshSldr,
		ID_AngleThreshText,

		//basic page
		//iterations
		ID_BasicIterSldr,
		ID_BasicIterText,
		//threshold
		ID_BasicThreshSldr,
		ID_BasicThreshText,
		//falloff
		ID_BasicDiffCheck,
		ID_BasicFalloffSldr,
		ID_BasicFalloffText,
		//size
		ID_BasicSizeCheck,
		ID_BasicSizeSldr,
		ID_BasicSizeText,
		//density
		ID_BasicDensityCheck,
		ID_BasicDensitySldr,
		ID_BasicDensityText,
		//clean
		ID_BasicCleanCheck,
		ID_BasicCleanIterSldr,
		ID_BasicCleanIterText,
		ID_BasicCleanLimitSldr,
		ID_BasicCleanLimitText,

		//clustering page
		ID_ClusterMethodExmaxRd,
		ID_ClusterMethodDbscanRd,
		ID_ClusterMethodKmeansRd,
		//parameters
		ID_ClusterClnumSldr,
		ID_ClusterClnumText,
		ID_ClusterMaxIterSldr,
		ID_ClusterMaxIterText,
		ID_ClusterSizeSldr,
		ID_ClusterSizeText,
		ID_ClusterEpsSldr,
		ID_ClusterEpsText,

		//analysis page
		//selection
		ID_CompIdText,
		ID_CompIdXBtn,
		ID_AnalysisMinCheck,
		ID_AnalysisMinSpin,
		ID_AnalysisMaxCheck,
		ID_AnalysisMaxSpin,
		ID_CompFullBtn,
		ID_CompExclusiveBtn,
		ID_CompAppendBtn,
		ID_CompAllBtn,
		ID_CompClearBtn,
		//output
		ID_OutputMultiRb,
		ID_OutputRgbRb,
		ID_OutputRandomBtn,
		ID_OutputSizeBtn,
		ID_OutputAnnBtn,

		//execute
		ID_Notebook,
		ID_UseSelChk,
		ID_GeneratePrg,
		ID_GenerateBtn,
		ID_RefineBtn,
		ID_ClusterBtn,
		ID_AnalyzeBtn,
		ID_AnalyzeSelBtn,

		//output
		ID_StatText
	};

	ComponentDlg(wxWindow* frame,
		wxWindow* parent);
	~ComponentDlg();

	void Update();
	void GetSettings();
	void LoadSettings(wxString filename);
	void SaveSettings(wxString filename);
	void SetView(VRenderView* vrv) {
		m_view = vrv;
	}
	VRenderView* GetView() {
		return m_view;
	}

	//update progress
	void UpdateProgress()
	{
		m_prog += m_prog_bit;
		m_generate_prg->SetValue(int(m_prog));
		wxGetApp().Yield();
	}

	//type: 0-basic; 1-advanced
	//mode: 0-generate; 1-refine
	void GenerateComp(int type, int mode);

private:
	wxWindow* m_frame;
	VRenderView* m_view;

	//progress
	float m_prog_bit;
	float m_prog;

	//initial grow
	bool m_initial_grow;
	bool m_ig_param_transition;
	int m_ig_iterations;
	//translate
	double m_ig_translate;
	double m_ig_translate2;
	//scalar falloff
	double m_ig_scalar_falloff;
	double m_ig_scalar_falloff2;
	//grad falloff
	double m_ig_grad_falloff;
	double m_ig_grad_falloff2;
	//variance falloff
	double m_ig_var_falloff;
	double m_ig_var_falloff2;
	//angle falloff
	double m_ig_angle_falloff;
	double m_ig_angle_falloff2;

	//sized grow
	bool m_sized_grow;
	bool m_sg_param_transition;
	int m_sg_iterations;
	//size limiter
	int m_sg_size_limiter;
	int m_sg_size_limiter2;
	//translate
	double m_sg_translate;
	double m_sg_translate2;
	//scalar falloff
	double m_sg_scalar_falloff;
	double m_sg_scalar_falloff2;
	//grad falloff
	double m_sg_grad_falloff;
	double m_sg_grad_falloff2;
	//variance falloff
	double m_sg_var_falloff;
	double m_sg_var_falloff2;
	//angle falloff
	double m_sg_angle_falloff;
	double m_sg_angle_falloff2;

	//cleanup
	bool m_cleanup;
	int m_cl_iterations;
	int m_cl_size_limiter;
	
	//match slices
	bool m_match_slices;
	bool m_bidir_match;
	int m_size_thresh;
	double m_size_ratio;
	double m_dist_thresh;
	double m_angle_thresh;

	//basic settings
	int m_basic_iter;
	double m_basic_thresh;
	bool m_basic_diff;
	double m_basic_falloff;
	bool m_basic_size;
	int m_basic_size_lm;
	//density
	bool m_basic_density;
	double m_basic_density_vl;
	//clean
	bool m_basic_clean;
	int m_basic_clean_iter;
	int m_basic_clean_size_vl;

	//cluster settings
	bool m_cluster_method_exmax;
	bool m_cluster_method_dbscan;
	bool m_cluster_method_kmeans;
	//parameters
	int m_cluster_clnum;
	int m_cluster_maxiter;
	int m_cluster_size;
	double m_cluster_eps;

	//selection
	bool m_use_min;
	int m_min_num;
	bool m_use_max;
	int m_max_num;

	//output
	int m_output_type;//1-multi; 2-rgb;

	FL::ComponentAnalyzer m_comp_analyzer;

	//tab control
	wxNotebook *m_notebook;
	wxScrolledWindow* m_adv_page;

	//load/save settings
	wxTextCtrl* m_load_settings_text;
	wxButton* m_load_settings_btn;
	wxButton* m_save_settings_btn;
	wxButton* m_saveas_settings_btn;

	//initial grow pane
	wxCollapsiblePane* m_initial_grow_pane;
	wxCheckBox* m_initial_grow_check;
	wxCheckBox* m_ig_param_transition_check;
	wxSlider* m_ig_iterations_sldr;
	wxTextCtrl* m_ig_iterations_text;
	//translate
	wxSlider* m_ig_translate_sldr;
	wxTextCtrl* m_ig_translate_text;
	wxStaticText* m_ig_translate_st;
	wxSlider* m_ig_translate2_sldr;
	wxTextCtrl* m_ig_translate2_text;
	//scalar falloff
	wxSlider* m_ig_scalar_falloff_sldr;
	wxTextCtrl* m_ig_scalar_falloff_text;
	wxStaticText* m_ig_scalar_falloff_st;
	wxSlider* m_ig_scalar_falloff2_sldr;
	wxTextCtrl* m_ig_scalar_falloff2_text;
	//grad falloff
	wxSlider* m_ig_grad_falloff_sldr;
	wxTextCtrl* m_ig_grad_falloff_text;
	wxStaticText* m_ig_grad_falloff_st;
	wxSlider* m_ig_grad_falloff2_sldr;
	wxTextCtrl* m_ig_grad_falloff2_text;
	//variance falloff
	wxSlider* m_ig_var_falloff_sldr;
	wxTextCtrl* m_ig_var_falloff_text;
	wxStaticText* m_ig_var_falloff_st;
	wxSlider* m_ig_var_falloff2_sldr;
	wxTextCtrl* m_ig_var_falloff2_text;
	//angle falloff
	wxSlider* m_ig_angle_falloff_sldr;
	wxTextCtrl* m_ig_angle_falloff_text;
	wxStaticText* m_ig_angle_falloff_st;
	wxSlider* m_ig_angle_falloff2_sldr;
	wxTextCtrl* m_ig_angle_falloff2_text;

	//sized grow pane
	wxCollapsiblePane* m_sized_grow_pane;
	wxCheckBox* m_sized_grow_check;
	wxCheckBox* m_sg_param_transition_check;
	wxSlider* m_sg_iterations_sldr;
	wxTextCtrl* m_sg_iterations_text;
	//size limiter
	wxSlider* m_sg_size_limiter_sldr;
	wxTextCtrl* m_sg_size_limiter_text;
	wxStaticText* m_sg_size_limiter_st;
	wxSlider* m_sg_size_limiter2_sldr;
	wxTextCtrl* m_sg_size_limiter2_text;
	//translate
	wxSlider* m_sg_translate_sldr;
	wxTextCtrl* m_sg_translate_text;
	wxStaticText* m_sg_translate_st;
	wxSlider* m_sg_translate2_sldr;
	wxTextCtrl* m_sg_translate2_text;
	//scalar falloff
	wxSlider* m_sg_scalar_falloff_sldr;
	wxTextCtrl* m_sg_scalar_falloff_text;
	wxStaticText* m_sg_scalar_falloff_st;
	wxSlider* m_sg_scalar_falloff2_sldr;
	wxTextCtrl* m_sg_scalar_falloff2_text;
	//grad falloff
	wxSlider* m_sg_grad_falloff_sldr;
	wxTextCtrl* m_sg_grad_falloff_text;
	wxStaticText* m_sg_grad_falloff_st;
	wxSlider* m_sg_grad_falloff2_sldr;
	wxTextCtrl* m_sg_grad_falloff2_text;
	//variance falloff
	wxSlider* m_sg_var_falloff_sldr;
	wxTextCtrl* m_sg_var_falloff_text;
	wxStaticText* m_sg_var_falloff_st;
	wxSlider* m_sg_var_falloff2_sldr;
	wxTextCtrl* m_sg_var_falloff2_text;
	//angle falloff
	wxSlider* m_sg_angle_falloff_sldr;
	wxTextCtrl* m_sg_angle_falloff_text;
	wxStaticText* m_sg_angle_falloff_st;
	wxSlider* m_sg_angle_falloff2_sldr;
	wxTextCtrl* m_sg_angle_falloff2_text;

	//cleanup
	wxCollapsiblePane* m_cleanup_pane;
	wxCheckBox* m_cleanup_check;
	wxSlider* m_cl_iterations_sldr;
	wxTextCtrl* m_cl_iterations_text;
	wxSlider* m_cl_size_limiter_sldr;
	wxTextCtrl* m_cl_size_limiter_text;

	//match slices
	wxCollapsiblePane* m_match_slices_pane;
	wxCheckBox* m_match_slices_check;
	wxCheckBox* m_bidir_match_check;
	wxSlider* m_size_thresh_sldr;
	wxTextCtrl* m_size_thresh_text;
	wxSlider* m_size_ratio_sldr;
	wxTextCtrl* m_size_ratio_text;
	wxSlider* m_dist_thresh_sldr;
	wxTextCtrl* m_dist_thresh_text;
	wxSlider* m_angle_thresh_sldr;
	wxTextCtrl* m_angle_thresh_text;

	//basic page
	wxSlider* m_basic_iter_sldr;
	wxTextCtrl* m_basic_iter_text;
	wxSlider* m_basic_thresh_sldr;
	wxTextCtrl* m_basic_thresh_text;
	wxCheckBox* m_basic_diff_check;
	wxSlider* m_basic_falloff_sldr;
	wxTextCtrl* m_basic_falloff_text;
	wxCheckBox* m_basic_size_check;
	wxSlider* m_basic_size_sldr;
	wxTextCtrl* m_basic_size_text;
	//density
	wxCheckBox* m_basic_density_check;
	wxSlider* m_basic_density_sldr;
	wxTextCtrl* m_basic_density_text;
	//clean
	wxCheckBox* m_basic_clean_check;
	wxSlider* m_basic_clean_iter_sldr;
	wxTextCtrl* m_basic_clean_iter_text;
	wxSlider* m_basic_clean_limit_sldr;
	wxTextCtrl* m_basic_clean_limit_text;

	//clustering page
	wxRadioButton* m_cluster_method_exmax_rd;
	wxRadioButton* m_cluster_method_dbscan_rd;
	wxRadioButton* m_cluster_method_kmeans_rd;
	//parameters
	wxSlider* m_cluster_clnum_sldr;
	wxTextCtrl* m_cluster_clnum_text;
	wxSlider* m_cluster_maxiter_sldr;
	wxTextCtrl* m_cluster_maxiter_text;
	wxSlider* m_cluster_size_sldr;
	wxTextCtrl* m_cluster_size_text;
	wxSlider* m_cluster_eps_sldr;
	wxTextCtrl* m_cluster_eps_text;

	//analysis page
	//selection
	wxTextCtrl* m_comp_id_text;
	wxButton* m_comp_id_x_btn;
	wxButton* m_comp_full_btn;
	wxButton* m_comp_exclusive_btn;
	wxButton* m_comp_append_btn;
	wxButton* m_comp_all_btn;
	wxButton* m_comp_clear_btn;
	//stats
	wxCheckBox* m_analysis_min_check;
	wxSpinCtrl* m_analysis_min_spin;
	wxCheckBox* m_analysis_max_check;
	wxSpinCtrl* m_analysis_max_spin;
	//output
	wxRadioButton* m_output_multi_rb;
	wxRadioButton* m_output_rgb_rb;
	wxButton* m_output_random_btn;
	wxButton* m_output_size_btn;
	wxButton* m_output_ann_btn;

	//execute
	wxGauge* m_generate_prg;
	wxCheckBox* m_use_sel_chk;
	wxButton* m_generate_btn;
	wxButton* m_refine_btn;
	wxButton* m_cluster_btn;
	wxButton* m_analyze_btn;
	wxButton* m_analyze_sel_btn;

	//output
	wxTextCtrl* m_stat_text;

private:
	void GenerateAdv(bool refine);
	void GenerateBsc(bool refine);
	void Cluster();

	wxWindow* Create3DAnalysisPage(wxWindow *parent);
	wxWindow* Create2DAnalysisPage(wxWindow *parent);
	wxWindow* CreateClusteringPage(wxWindow *parent);
	wxWindow* CreateAnalysisPage(wxWindow *parent);
	void OnPaneChange(wxCollapsiblePaneEvent& event);
	wxCollapsiblePane* CreateInitialGrowPane(wxWindow *parent);
	wxCollapsiblePane* CreateSizedGrowPane(wxWindow *parent);
	wxCollapsiblePane* CreateCleanupPane(wxWindow *parent);
	wxCollapsiblePane* CreateMatchSlicesPane(wxWindow *parent);

	//load/save settings
	void OnLoadSettings(wxCommandEvent &event);
	void OnSaveSettings(wxCommandEvent &event);
	void OnSaveasSettings(wxCommandEvent &event);

	//initial grow pane
	void EnableInitialGrow(bool value);
	void EnableIGParamTransition(bool value);
	void OnInitialGrowCheck(wxCommandEvent &event);
	void OnIGParamTransitionCheck(wxCommandEvent &event);
	void OnIGIterationsSldr(wxScrollEvent &event);
	void OnIGIterationsText(wxCommandEvent &event);
	//translate
	void OnIGTranslateSldr(wxScrollEvent &event);
	void OnIGTranslateText(wxCommandEvent &event);
	void OnIGTranslate2Sldr(wxScrollEvent &event);
	void OnIGTranslate2Text(wxCommandEvent &event);
	//scalar falloff
	void OnIGScalarFalloffSldr(wxScrollEvent &event);
	void OnIGScalarFalloffText(wxCommandEvent &event);
	void OnIGScalarFalloff2Sldr(wxScrollEvent &event);
	void OnIGScalarFalloff2Text(wxCommandEvent &event);
	//grad falloff
	void OnIGGradFalloffSldr(wxScrollEvent &event);
	void OnIGGradFalloffText(wxCommandEvent &event);
	void OnIGGradFalloff2Sldr(wxScrollEvent &event);
	void OnIGGradFalloff2Text(wxCommandEvent &event);
	//variance falloff
	void OnIGVarFalloffSldr(wxScrollEvent &event);
	void OnIGVarFalloffText(wxCommandEvent &event);
	void OnIGVarFalloff2Sldr(wxScrollEvent &event);
	void OnIGVarFalloff2Text(wxCommandEvent &event);
	//angle falloff
	void OnIGAngleFalloffSldr(wxScrollEvent &event);
	void OnIGAngleFalloffText(wxCommandEvent &event);
	void OnIGAngleFalloff2Sldr(wxScrollEvent &event);
	void OnIGAngleFalloff2Text(wxCommandEvent &event);

	//sized grow pane
	void EnableSizedGrow(bool value);
	void EnableSGParamTransition(bool value);
	void OnSizedGrowCheck(wxCommandEvent &event);
	void OnSGParamTransitionCheck(wxCommandEvent &event);
	void OnSGIterationsSldr(wxScrollEvent &event);
	void OnSGIterationsText(wxCommandEvent &event);
	//size limiter
	void OnSGSizeLimiterSldr(wxScrollEvent &event);
	void OnSGSizeLimiterText(wxCommandEvent &event);
	void OnSGSizeLimiter2Sldr(wxScrollEvent &event);
	void OnSGSizeLimiter2Text(wxCommandEvent &event);
	//translate
	void OnSGTranslateSldr(wxScrollEvent &event);
	void OnSGTranslateText(wxCommandEvent &event);
	void OnSGTranslate2Sldr(wxScrollEvent &event);
	void OnSGTranslate2Text(wxCommandEvent &event);
	//scalar falloff
	void OnSGScalarFalloffSldr(wxScrollEvent &event);
	void OnSGScalarFalloffText(wxCommandEvent &event);
	void OnSGScalarFalloff2Sldr(wxScrollEvent &event);
	void OnSGScalarFalloff2Text(wxCommandEvent &event);
	//grad falloff
	void OnSGGradFalloffSldr(wxScrollEvent &event);
	void OnSGGradFalloffText(wxCommandEvent &event);
	void OnSGGradFalloff2Sldr(wxScrollEvent &event);
	void OnSGGradFalloff2Text(wxCommandEvent &event);
	//variance falloff
	void OnSGVarFalloffSldr(wxScrollEvent &event);
	void OnSGVarFalloffText(wxCommandEvent &event);
	void OnSGVarFalloff2Sldr(wxScrollEvent &event);
	void OnSGVarFalloff2Text(wxCommandEvent &event);
	//angle falloff
	void OnSGAngleFalloffSldr(wxScrollEvent &event);
	void OnSGAngleFalloffText(wxCommandEvent &event);
	void OnSGAngleFalloff2Sldr(wxScrollEvent &event);
	void OnSGAngleFalloff2Text(wxCommandEvent &event);

	//cleanup pane
	void EnableCleanup(bool value);
	void OnCleanupCheck(wxCommandEvent &event);
	void OnCLIterationsSldr(wxScrollEvent &event);
	void OnCLIterationsText(wxCommandEvent &event);
	void OnCLSizeLimiterSldr(wxScrollEvent &event);
	void OnCLSizeLimiterText(wxCommandEvent &event);

	//match slices pane
	void EnableMatchSlices(bool value);
	void OnMatchSlicesCheck(wxCommandEvent &event);
	void OnBidirMatchCheck(wxCommandEvent &event);
	void OnSizeThreshSldr(wxScrollEvent &event);
	void OnSizeThreshText(wxCommandEvent &event);
	void OnSizeRatioSldr(wxScrollEvent &event);
	void OnSizeRatioText(wxCommandEvent &event);
	void OnDistThreshSldr(wxScrollEvent &event);
	void OnDistThreshText(wxCommandEvent &event);
	void OnAngleThreshSldr(wxScrollEvent &event);
	void OnAngleThreshText(wxCommandEvent &event);

	//basic page
	void OnBasicIterSldr(wxScrollEvent &event);
	void OnBasicIterText(wxCommandEvent &event);
	void OnBasicThreshSldr(wxScrollEvent &event);
	void OnBasicThreshText(wxCommandEvent &event);
	void EnableBasicDiff(bool value);
	void OnBasicDiffCheck(wxCommandEvent &event);
	void OnBasicFalloffSldr(wxScrollEvent &event);
	void OnBasicFalloffText(wxCommandEvent &event);
	void EnableBasicSize(bool value);
	void OnBasicSizeCheck(wxCommandEvent &event);
	void OnBasicSizeSldr(wxScrollEvent &event);
	void OnBasicSizeText(wxCommandEvent &event);
	//density
	void EnableBasicDensity(bool value);
	void OnBasicDensityCheck(wxCommandEvent &event);
	void OnBasicDensitySldr(wxScrollEvent &event);
	void OnBasicDensityText(wxCommandEvent &event);
	//clean
	void EnableBasicClean(bool value);
	void OnBasicCleanCheck(wxCommandEvent &event);
	void OnBasicCleanIterSldr(wxScrollEvent &event);
	void OnBasicCleanIterText(wxCommandEvent &event);
	void OnBasicCleanLimitSldr(wxScrollEvent &event);
	void OnBasicCleanLimitText(wxCommandEvent &event);

	//clustering page
	void UpdateClusterMethod();
	void OnClusterMethodExmaxCheck(wxCommandEvent &event);
	void OnClusterMethodDbscanCheck(wxCommandEvent &event);
	void OnClusterMethodKmeansCheck(wxCommandEvent &event);
	//parameters
	void OnClusterClnumSldr(wxScrollEvent &event);
	void OnClusterClnumText(wxCommandEvent &event);
	void OnClusterMaxiterSldr(wxScrollEvent &event);
	void OnClusterMaxiterText(wxCommandEvent &event);
	void OnClusterSizeSldr(wxScrollEvent &event);
	void OnClusterSizeText(wxCommandEvent &event);
	void OnClusterEpsSldr(wxScrollEvent &event);
	void OnClusterepsText(wxCommandEvent &event);

	//analysis page
	void OnCompIdText(wxCommandEvent &event);
	void OnCompIdXBtn(wxCommandEvent &event);
	void OnAnalysisMinCheck(wxCommandEvent &event);
	void OnAnalysisMinSpin(wxSpinEvent &event);
	void OnAnalysisMaxCheck(wxCommandEvent &event);
	void OnAnalysisMaxSpin(wxSpinEvent &event);
	void OnCompFull(wxCommandEvent &event);
	void OnCompExclusive(wxCommandEvent &event);
	void OnCompAppend(wxCommandEvent &event);
	void OnCompAll(wxCommandEvent &event);
	void OnCompClear(wxCommandEvent &event);
	//output
	void OnOutputTypeRadio(wxCommandEvent &event);
	void OutputMulti(int color_type);
	void OutputRgb(int color_type);
	void OnOutputChannels(wxCommandEvent &event);
	void OnOutputAnn(wxCommandEvent &event);

	//execute
	void EnableGenerate();
	void OnNotebook(wxBookCtrlEvent &event);
	void OnGenerate(wxCommandEvent &event);
	void OnRefine(wxCommandEvent &event);
	void OnCluster(wxCommandEvent &event);
	void OnAnalyze(wxCommandEvent &event);
	void OnAnalyzeSel(wxCommandEvent &event);

	DECLARE_EVENT_TABLE();
};

#endif//_COMPONENTDLG_H_