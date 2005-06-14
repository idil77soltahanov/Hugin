// -*- c-basic-offset: 4 -*-

/** @file PreferencesDialog.cpp
 *
 *  @brief implementation of PreferencesDialog Class
 *
 *  @author Pablo d'Angelo <pablo.dangelo@web.de>
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <config.h>
#include "panoinc_WX.h"
#include "panoinc.h"

#ifdef HAVE_PANO12_QUERYFEATURE_H
#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

extern "C" {
#include <pano12/version.h>
#include <pano12/queryfeature.h>
}
#endif

#include "common/wxPlatform.h"

#include "hugin/huginApp.h"
#include "hugin/config_defaults.h"
#include "hugin/PreferencesDialog.h"

// validators are working different somehow...
//#define MY_STR_VAL(id, filter) { XRCCTRL(*this, "prefs_" #id, wxTextCtrl)->SetValidator(wxTextValidator(filter, &id)); }
//#define MY_SPIN_VAL(id) {     XRCCTRL(*this, "prefs_" #id, wxSpinCtrl)->SetValidator(wxGenericValidator(&id)); }

#define MY_STR_VAL(id, val) { XRCCTRL(*this, id, wxTextCtrl)->SetValue(val); };
#define MY_SPIN_VAL(id, val) { XRCCTRL(*this, id, wxSpinCtrl)->SetValue(val); };
#define MY_BOOL_VAL(id, val) { XRCCTRL(*this, id, wxCheckBox)->SetValue(val); };
#define MY_CHOICE_VAL(id, val) { XRCCTRL(*this, id, wxChoice)->SetSelection(val); };
#define MY_STATIC_VAL(id, val) { XRCCTRL(*this, id, wxStaticText)->SetLabel(val); };

#define MY_G_STR_VAL(id)  XRCCTRL(*this, id, wxTextCtrl)->GetValue()
#define MY_G_SPIN_VAL(id)  XRCCTRL(*this, id, wxSpinCtrl)->GetValue()
#define MY_G_BOOL_VAL(id)  XRCCTRL(*this, id, wxCheckBox)->GetValue()
#define MY_G_CHOICE_VAL(id)  XRCCTRL(*this, id, wxChoice)->GetSelection()



BEGIN_EVENT_TABLE(PreferencesDialog, wxFrame)
    EVT_CLOSE(PreferencesDialog::OnClose)
    EVT_BUTTON(wxID_OK, PreferencesDialog::OnOk)
    EVT_BUTTON(wxID_APPLY,PreferencesDialog::OnApply)
    EVT_BUTTON(wxID_CANCEL, PreferencesDialog::OnCancel)
    EVT_BUTTON(XRCID("prefs_ptstitcher_select"), PreferencesDialog::OnPTStitcherExe)
    EVT_BUTTON(XRCID("prefs_ptoptimizer_select"), PreferencesDialog::OnPTOptimizerExe)
    EVT_BUTTON(XRCID("prefs_enblend_select"), PreferencesDialog::OnEnblendExe)
    EVT_BUTTON(XRCID("prefs_AutoPanoKolorExe_select"), PreferencesDialog::OnAutopanoKolorExe)
    EVT_BUTTON(XRCID("prefs_AutoPanoSIFTExe_select"), PreferencesDialog::OnAutopanoSiftExe)
    EVT_BUTTON(XRCID("prefs_load_defaults"), PreferencesDialog::OnDefaults)
    EVT_BUTTON(XRCID("prefs_panotools_details"), PreferencesDialog::OnPTDetails)
    EVT_CHECKBOX(XRCID("prefs_ft_RotationSearch"), PreferencesDialog::OnRotationCheckBox)
//    EVT_CLOSE(RunOptimizerFrame::OnClose)
END_EVENT_TABLE()


PreferencesDialog::PreferencesDialog(wxWindow *parent)
//    : wxFrame(parent, -1, _("Preferences - hugin"))
{
    DEBUG_TRACE("");
    // load our children. some children might need special
    // initialization. this will be done later.
    wxXmlResource::Get()->LoadFrame(this, parent, wxT("pref_dialog"));

#if __WXMSW__
    wxIcon myIcon(MainFrame::Get()->GetXRCPath() + wxT("data/icon.ico"),wxBITMAP_TYPE_ICO);
#else
    wxIcon myIcon(MainFrame::Get()->GetXRCPath() + wxT("data/icon.png"),wxBITMAP_TYPE_PNG);
#endif
    SetIcon(myIcon);

    // Custom setup ( stuff that can not be done in XRC )
    XRCCTRL(*this, "prefs_ft_RotationStartAngle", wxSpinCtrl)->SetRange(-180,0);
    XRCCTRL(*this, "prefs_ft_RotationStopAngle", wxSpinCtrl)->SetRange(0,180);

    wxChoice *lang_choice = XRCCTRL(*this, "prefs_gui_language", wxChoice);

    // add languages to choice
    lang_choice->Append(_("Select one"), (void *) huginApp::Get()->GetLocale().GetLanguage());
    lang_choice->Append(_("System default"), (void *) wxLANGUAGE_DEFAULT);
    lang_choice->Append(_("English"), (void *) wxLANGUAGE_ENGLISH);
    lang_choice->Append(_("German"), (void *) wxLANGUAGE_GERMAN);
    lang_choice->Append(_("French"), (void *) wxLANGUAGE_FRENCH);
    lang_choice->Append(_("Polish"), (void *) wxLANGUAGE_POLISH);
    lang_choice->Append(_("Italian"), (void *) wxLANGUAGE_ITALIAN);
    lang_choice->Append(_("Japanese"), (void *) wxLANGUAGE_JAPANESE);
    lang_choice->SetSelection(0);

    // Load configuration values from wxConfig
    UpdateDisplayData();
}


PreferencesDialog::~PreferencesDialog()
{
    DEBUG_TRACE("begin dtor");
    DEBUG_TRACE("end dtor");
}

void PreferencesDialog::OnApply(wxCommandEvent & e)
{
    UpdateConfigData();
}

void PreferencesDialog::OnOk(wxCommandEvent & e)
{
    UpdateConfigData();
    DEBUG_DEBUG("show false");
    this->Show(FALSE);
}

void PreferencesDialog::OnCancel(wxCommandEvent & e)
{
    UpdateDisplayData();
    DEBUG_DEBUG("show false");
    this->Show(FALSE);
}

void PreferencesDialog::OnClose(wxCloseEvent& event)
{
    DEBUG_DEBUG("OnClose");
    // do not close, just hide if we're not forced
    if (event.CanVeto()) {
        event.Veto();
        Hide();
        DEBUG_DEBUG("hiding");
    } else {
        DEBUG_DEBUG("about to destroy");
        Destroy();
        DEBUG_DEBUG("destroyed");
    }
}

void PreferencesDialog::OnDefaults(wxCommandEvent & e)
{
    DEBUG_WARN("Not implemented yet");
}

void PreferencesDialog::OnRotationCheckBox(wxCommandEvent & e)
{
    EnableRotationCtrls(e.IsChecked());
}

void PreferencesDialog::OnPTStitcherExe(wxCommandEvent & e)
{
    wxFileDialog dlg(this,_("Select PTStitcher"),
	             wxT(""), wxT(HUGIN_PT_STITCHER_EXE),
#ifdef __WXMSW__
		     _("Executables (*.exe)|*.exe"),
#else
		     wxT(""),
#endif
                    wxOPEN, wxDefaultPosition);
    if (dlg.ShowModal() == wxID_OK) {
	XRCCTRL(*this, "prefs_pt_PTStitcherEXE", wxTextCtrl)->SetValue(
		dlg.GetPath());
    }

}
void PreferencesDialog::OnPTOptimizerExe(wxCommandEvent & e)
{
    wxFileDialog dlg(this,_("Select PTOptimizer"),
	             wxT(""), wxT(HUGIN_PT_OPTIMIZER_EXE),
#ifdef __WXMSW__
		     _("Executables (*.exe)|*.exe"),
#else
		     wxT(""),
#endif
                    wxOPEN, wxDefaultPosition);
    if (dlg.ShowModal() == wxID_OK) {
	XRCCTRL(*this, "prefs_pt_PTOptimizerEXE", wxTextCtrl)->SetValue(
		dlg.GetPath());
    }
}

void PreferencesDialog::OnEnblendExe(wxCommandEvent & e)
{
    wxFileDialog dlg(this,_("Select Enblend"),
	             wxT(""), wxT(HUGIN_ENBLEND_EXE),
#ifdef __WXMSW__
		     _("Executables (*.exe)|*.exe"),
#else
		     wxT(""),
#endif
                    wxOPEN, wxDefaultPosition);
    if (dlg.ShowModal() == wxID_OK) {
	XRCCTRL(*this, "prefs_enblend_EnblendExe", wxTextCtrl)->SetValue(
		dlg.GetPath());
    }
}

void PreferencesDialog::OnAutopanoKolorExe(wxCommandEvent & e)
{
    wxFileDialog dlg(this,_("Select Autopano"),
	             wxT(""), wxT(HUGIN_APKOLOR_EXE),
#ifdef __WXMSW__
		     _("Executables (*.exe)|*.exe"),
#else
		     wxT(""),
#endif
                    wxOPEN, wxDefaultPosition);
    if (dlg.ShowModal() == wxID_OK) {
	XRCCTRL(*this, "prefs_AutoPanoKolorExe", wxTextCtrl)->SetValue(
		dlg.GetPath());
    }
}

void PreferencesDialog::OnAutopanoSiftExe(wxCommandEvent & e)
{
    wxFileDialog dlg(this,_("Select Autopano"),
	             wxT(""), wxT(HUGIN_APSIFT_EXE),
#ifdef __WXMSW__
		     _("Executables (*.exe,*.vbs,*.cmd)|*.exe;*.vbs;*.cmd"),
#else
		     wxT(""),
#endif
                    wxOPEN, wxDefaultPosition);
    if (dlg.ShowModal() == wxID_OK) {
	XRCCTRL(*this, "prefs_AutoPanoSIFTExe", wxTextCtrl)->SetValue(
		dlg.GetPath());
    }
}

void PreferencesDialog::OnPTDetails(wxCommandEvent & e)
{
	DEBUG_INFO("Panotools Details Requested:\n" << m_PTDetails.mb_str());
    
#ifdef __WXMAC__ // Limiting the dialog to first 25 lines (apprrox. height of display)
    wxString temp1 = m_PTDetails;
    wxString temp2 = wxT("");
    
    if(m_PTDetails.Freq('\n') > 25)
    {
        for(int i = 0; i < 25; i++)
        {
            temp2 += temp1.BeforeFirst('\n') + wxT("\n");
            temp1 = temp1.AfterFirst('\n');
        }
        temp2 += wxT("\n\t...");
    } else {
        temp2 = m_PTDetails;
    }
    
	wxMessageDialog dlg(this, temp2, _("Panotools details"), wxOK|wxICON_INFORMATION);
#else
	wxMessageDialog dlg(this, m_PTDetails, _("Panotools details"), wxOK);
#endif
	dlg.ShowModal();
}

void PreferencesDialog::EnableRotationCtrls(bool enable)
{
    XRCCTRL(*this, "prefs_ft_rot_panel", wxPanel)->Enable(enable);
}

#ifdef HAVE_PANO12_QUERYFEATURE_H
typedef int (*PROC_QF)			(int ,char** ,Tp12FeatureType* );
typedef int (*PROC_QFNUM)		(void);
typedef int (*PROC_QFINT)		(const char *, int *); 
typedef int (*PROC_QFDOUBLE)	(const char *, double *);
typedef int (*PROC_QFSTRING)	(const char *, char *, const int);
#endif

bool PreferencesDialog::GetPanoVersion()
{

        
#ifdef HAVE_PANO12_QUERYFEATURE_H
#ifdef __WXMSW__
	HINSTANCE		hDll		= NULL;
#else
	void *hDll = NULL;
#endif
	PROC_QF			pfQF		= NULL;
	PROC_QFNUM		pfQFNum		= NULL;
	PROC_QFINT		pfQFInt		= NULL;
	PROC_QFDOUBLE	pfQFDouble	= NULL;
	PROC_QFSTRING	pfQFString	= NULL;

	int				iResult;
	double			dResult;
	char			sResult[256];
	char			str1[1000];
	char			str2[10000];
	bool			bSuccess = true;

#ifdef __WXMSW__
	hDll = LoadLibrary(_T("pano12.dll"));
	if(!hDll)
	{
		MessageBox((HWND)NULL, _("Could not load dll"), _("panoinfo"), MB_ICONEXCLAMATION);
		bSuccess = false;
		goto cleanup;
	}

	pfQF		= (PROC_QF) GetProcAddress( hDll, "queryFeatures" );
	pfQFNum		= (PROC_QFNUM) GetProcAddress( hDll, "queryFeatureCount" );
	pfQFInt     = (PROC_QFINT) GetProcAddress( hDll, "queryFeatureInt" );
	pfQFDouble  = (PROC_QFDOUBLE) GetProcAddress( hDll, "queryFeatureDouble" );
	pfQFString  = (PROC_QFSTRING) GetProcAddress( hDll, "queryFeatureString" );
#elif (defined __WXMAC__)
//HuginOSX has libpano12 statically linked; hard code the query functions instead
	pfQF		= (PROC_QF) queryFeatures;
	pfQFNum		= (PROC_QFNUM) queryFeatureCount;
	pfQFInt     = (PROC_QFINT) queryFeatureInt;
	pfQFDouble  = (PROC_QFDOUBLE) queryFeatureDouble;
	pfQFString  = (PROC_QFSTRING) queryFeatureString;
#else
	hDll = dlopen("libpano12.so.0", RTLD_NOW);
	if(!hDll)
	{
		hDll = dlopen("libpano12.so", RTLD_NOW);
		if(!hDll)
		{
		  printf("Could not load pano12");
		  bSuccess = false;
		  goto cleanup;
		}
	}

	pfQF		= (PROC_QF) dlsym( hDll, "queryFeatures" );
	pfQFNum		= (PROC_QFNUM) dlsym( hDll, "queryFeatureCount" );
	pfQFInt     = (PROC_QFINT) dlsym( hDll, "queryFeatureInt" );
	pfQFDouble  = (PROC_QFDOUBLE) dlsym( hDll, "queryFeatureDouble" );
	pfQFString  = (PROC_QFSTRING) dlsym( hDll, "queryFeatureString" );
#endif
	str2[0] = '\0';
	if(!pfQF)
	{
		strcat(str2 ,"Error: The 'queryFeatures' function not present\n");
	}
	if(!pfQFNum)
	{
		strcat(str2 ,"Error: The 'queryFeatureCount' function not present\n");
	}
	if(!pfQFString)
	{
		strcat(str2 ,"Error: The 'queryFeatureString' function not present\n");
	}
	if(!pfQFInt)
	{
		strcat(str2 ,"Error: The 'queryFeatureInt' function not present\n");
	}
	if(!pfQFDouble)
	{
		strcat(str2 ,"Error: The 'queryFeatureDouble' function not present\n");
	}


	if(pfQFString)
	{
		if((pfQFString) (PTVERSION_NAME_FILEVERSION, sResult, sizeof(sResult)/sizeof(sResult[0]) ))
		{
			m_PTVersion = wxString(sResult, *wxConvCurrent);
		}

		if((pfQFString) (PTVERSION_NAME_COMMENT, sResult, sizeof(sResult)/sizeof(sResult[0]) ))
		{
			sprintf(str1, "Comment:\t%s\n", sResult );
			strcat(str2 ,str1);
		}

		if((pfQFString) (PTVERSION_NAME_LEGALCOPYRIGHT, sResult, sizeof(sResult)/sizeof(sResult[0]) ))
		{
			sprintf(str1, "Copyright:\t%s\n\n", sResult );
			strcat(str2 ,str1);
		}

	}

	if(pfQFInt)
	{
		if((pfQFInt) ("CPErrorIsDistSphere", &iResult ))
		{
			sprintf(str1, "Optimizer Error:\t%s\n", iResult? "dist sphere" : "dist rect" );
			strcat(str2 ,str1);
		}
	}

	if(pfQFDouble)
	{
		if((pfQFDouble) ("MaxFFOV", &dResult ))
		{
			sprintf(str1, "Max FoV:\t\t%f\n\n", dResult );
			strcat(str2 ,str1);
		}

	}

	if(pfQFNum && pfQF && pfQFString)
	{
		int i,bufsize,numfeatures;
		char *name;
		char *value;
		Tp12FeatureType type;

		strcat(str2 ,"Feature List:\n\n");

		numfeatures = pfQFNum();
		for(i=0; i < numfeatures;i++)
		{
			pfQF(i, &name, &type);
			bufsize = pfQFString(name, NULL, 0)+1;
			value = (char*)malloc(bufsize);
			pfQFString(name, value, bufsize);

			sprintf(str1, "   %s: %s\n", name, value);
			strcat(str2 ,str1);

			free(value);
		}
	}
	// use wxConvLocal to deal with the copyright symbols used by panotools
	m_PTDetails = wxString(str2, wxConvLocal);

cleanup:
  if (bSuccess)
  {
#ifdef __WXMSW__
	FreeLibrary(hDll);
#elif defined __WXMAC__
//nothing to do
#else
	dlclose(hDll);
#endif
	return true;
  } else {
	return false;
  }
#else // HAVE_PANO12_QUERYFEATURE_H
  return false;
#endif
}

void PreferencesDialog::UpdateDisplayData()
{

    DEBUG_DEBUG("Updating display data");

    double d;
    wxString tstr;
    wxConfigBase *cfg = wxConfigBase::Get();

    // Panotools settings
    MY_STR_VAL("prefs_pt_PTStitcherEXE", cfg->Read(wxT("/Panotools/PTStitcherExe"),wxT("PTStitcher")));
    MY_STR_VAL("prefs_pt_PTOptimizerEXE", cfg->Read(wxT("/Panotools/PTOptimizerExe"),wxT("PTOptimizer")));

    MY_STR_VAL("prefs_pt_ScriptFile", cfg->Read(wxT("/PanoTools/ScriptFile"),wxT("PT_script.txt")));

    // finetune settings
    MY_SPIN_VAL("prefs_ft_TemplateSize",
                cfg->Read(wxT("/Finetune/TemplateSize"),HUGIN_FT_TEMPLATE_SIZE));
    MY_SPIN_VAL("prefs_ft_SearchAreaPercent",cfg->Read(wxT("/Finetune/SearchAreaPercent"),
                                               HUGIN_FT_SEARCH_AREA_PERCENT));
    MY_SPIN_VAL("prefs_ft_LocalSearchWidth", cfg->Read(wxT("/Finetune/LocalSearchWidth"),
                                               HUGIN_FT_LOCAL_SEARCH_WIDTH));

    d=HUGIN_FT_CORR_THRESHOLD;
    cfg->Read(wxT("/Finetune/CorrThreshold"), &d, HUGIN_FT_CORR_THRESHOLD);
    tstr = utils::doubleTowxString(d);
    MY_STR_VAL("prefs_ft_CorrThreshold", tstr);

    cfg->Read(wxT("/Finetune/CurvThreshold"), &d, HUGIN_FT_CURV_THRESHOLD);
    tstr = utils::doubleTowxString(d);
    MY_STR_VAL("prefs_ft_CurvThreshold", tstr);

    bool t = cfg->Read(wxT("/Finetune/RotationSearch"), HUGIN_FT_ROTATION_SEARCH) == 1;
    MY_BOOL_VAL("prefs_ft_RotationSearch", t);
    EnableRotationCtrls(t);

    d = HUGIN_FT_ROTATION_START_ANGLE;
    cfg->Read(wxT("/Finetune/RotationStartAngle"),&d,HUGIN_FT_ROTATION_START_ANGLE);
    MY_SPIN_VAL("prefs_ft_RotationStartAngle", utils::roundi(d))

    d = HUGIN_FT_ROTATION_STOP_ANGLE;
    cfg->Read(wxT("/Finetune/RotationStopAngle"), &d, HUGIN_FT_ROTATION_STOP_ANGLE);
    MY_SPIN_VAL("prefs_ft_RotationStopAngle", utils::roundi(d));

    MY_SPIN_VAL("prefs_ft_RotationSteps", cfg->Read(wxT("/Finetune/RotationSteps"),
                                            HUGIN_FT_ROTATION_STEPS));

    /////
    /// MISC

    // memory setting
    long mem = cfg->Read(wxT("/ImageCache/UpperBound"), HUGIN_IMGCACHE_UPPERBOUND);
    MY_SPIN_VAL("prefs_cache_UpperBound", mem >> 20);

    // cursor setting
    mem = cfg->Read(wxT("/CPImageCtrl/CursorType"), HUGIN_CP_CURSOR);
    MY_SPIN_VAL("prefs_cp_CursorType", mem);

    // tempdir
    MY_STR_VAL("prefs_misc_tempdir", cfg->Read(wxT("tempDir"),wxT("")));

    // show druid
    MY_BOOL_VAL("prefs_misc_showDruid", cfg->Read(wxT("/PreviewFrame/showDruid"),HUGIN_PREVIEW_SHOW_DRUID) != 0l);

    // use preview images as active images
    t = cfg->Read(wxT("/General/UseOnlySelectedImages"), HUGIN_USE_SELECTED_IMAGES) == 1;
    MY_BOOL_VAL("prefs_misc_UseSelectedImages", t);

    /////
    /// AUTOPANO

    // active autopano
    MY_CHOICE_VAL("prefs_AutoPanoType", cfg->Read(wxT("/AutoPano/Type"), HUGIN_AP_TYPE));

    // Autopano-SIFT
    MY_STR_VAL("prefs_AutoPanoSIFTExe", cfg->Read(wxT("/AutoPanoSift/AutopanoExe"),
                                                  wxT(HUGIN_APSIFT_EXE)));
    MY_STR_VAL("prefs_AutoPanoSIFTArgs", cfg->Read(wxT("/AutoPanoSift/Args"),
                                                   wxT(HUGIN_APSIFT_ARGS)));

    // Autopano
    MY_STR_VAL("prefs_AutoPanoKolorExe", cfg->Read(wxT("/AutoPanoKolor/AutopanoExe"),
                                                  wxT(HUGIN_APKOLOR_EXE)));
    MY_STR_VAL("prefs_AutoPanoKolorArgs", cfg->Read(wxT("/AutoPanoKolor/Args"),
                                                   wxT(HUGIN_APKOLOR_ARGS)));


    /////
    /// ENBLEND
    MY_STR_VAL("prefs_enblend_EnblendExe", cfg->Read(wxT("/Enblend/EnblendExe"),
                                                     wxT(HUGIN_ENBLEND_EXE)));
    MY_STR_VAL("prefs_enblend_EnblendArgs", cfg->Read(wxT("/Enblend/EnblendArgs"),
                                                      wxT(HUGIN_ENBLEND_ARGS)));
    t = cfg->Read(wxT("/Enblend/DeleteRemappedFiles"), HUGIN_ENBLEND_DELETE_REMAPPED_FILES) == 1;
    MY_BOOL_VAL("prefs_enblend_DeleteRemapped", t);
    /////
	/// Display Panotools version if we can
	if (GetPanoVersion())
	{
  	  MY_STATIC_VAL("prefs_panotools_version", m_PTVersion);
	  XRCCTRL(*this, "prefs_panotools_details", wxButton)->Enable();
	} else
	{
  	  MY_STATIC_VAL("prefs_panotools_version", _("Unknown Version"));
	}
}

void PreferencesDialog::UpdateConfigData()
{
	DEBUG_TRACE("");
    wxConfigBase *cfg = wxConfigBase::Get();
    // Panotools settings

    cfg->Write(wxT("/Panotools/PTStitcherExe"), MY_G_STR_VAL("prefs_pt_PTStitcherEXE"));
    cfg->Write(wxT("/Panotools/PTOptimizerExe"), MY_G_STR_VAL("prefs_pt_PTOptimizerEXE"));
    cfg->Write(wxT("/PanoTools/ScriptFile"), MY_G_STR_VAL("prefs_pt_ScriptFile"));

    // Fine tune settings
    cfg->Write(wxT("/Finetune/SearchAreaPercent"), MY_G_SPIN_VAL("prefs_ft_SearchAreaPercent"));
    cfg->Write(wxT("/Finetune/TemplateSize"), MY_G_SPIN_VAL("prefs_ft_TemplateSize"));
    cfg->Write(wxT("/Finetune/LocalSearchWidth"), MY_G_SPIN_VAL("prefs_ft_LocalSearchWidth"));
    wxString t = MY_G_STR_VAL("prefs_ft_CorrThreshold");
    double td= HUGIN_FT_CORR_THRESHOLD;
    utils::stringToDouble(std::string(t.mb_str()), td);
    cfg->Write(wxT("/Finetune/CorrThreshold"), td);

    t = MY_G_STR_VAL("prefs_ft_CurvThreshold");
    td = HUGIN_FT_CURV_THRESHOLD;
    utils::stringToDouble(std::string(t.mb_str()), td);
    cfg->Write(wxT("/Finetune/CurvThreshold"), td);

    cfg->Write(wxT("/Finetune/RotationSearch"), MY_G_BOOL_VAL("prefs_ft_RotationSearch"));
    cfg->Write(wxT("/Finetune/RotationStartAngle"), (double) MY_G_SPIN_VAL("prefs_ft_RotationStartAngle"));
    cfg->Write(wxT("/Finetune/RotationStopAngle"), (double) MY_G_SPIN_VAL("prefs_ft_RotationStopAngle"));
    cfg->Write(wxT("/Finetune/RotationSteps"), MY_G_SPIN_VAL("prefs_ft_RotationSteps"));

    /////
    /// MISC
    // cache
    cfg->Write(wxT("/ImageCache/UpperBound"), MY_G_SPIN_VAL("prefs_cache_UpperBound") << 20);
    // locale
    // language
    wxChoice *lang = XRCCTRL(*this, "prefs_gui_language", wxChoice);
	DEBUG_INFO("Language Selection ID: " << (long)((int) lang->GetClientData(lang->GetSelection())));
	// DEBUG_TRACE("Language Selection Name: " << huginApp::Get()->GetLocale().GetLanguageName((int) lang->GetClientData(lang->GetSelection())).mb_str());
	DEBUG_INFO("Language Selection locale: " << ((huginApp::Get()->GetLocale().GetLanguageInfo((int) lang->GetClientData(lang->GetSelection())))->CanonicalName).mb_str());
	DEBUG_INFO("Current System Language ID: " << huginApp::Get()->GetLocale().GetSystemLanguage());

    cfg->Write(wxT("language"), (long)((int) lang->GetClientData(lang->GetSelection())));
    // cursor
    cfg->Write(wxT("/CPImageCtrl/CursorType"), MY_G_SPIN_VAL("prefs_cp_CursorType"));
    // tempdir
    cfg->Write(wxT("tempDir"),MY_G_STR_VAL("prefs_misc_tempdir"));
    // druid
    cfg->Write(wxT("/PreviewFrame/showDruid"), MY_G_BOOL_VAL("prefs_misc_showDruid"));
    // use preview images as active images
    cfg->Write(wxT("/General/UseOnlySelectedImages"), MY_G_BOOL_VAL("prefs_misc_UseSelectedImages"));

    /////
    /// AUTOPANO
    cfg->Write(wxT("/AutoPano/Type"),MY_G_CHOICE_VAL("prefs_AutoPanoType"));

    cfg->Write(wxT("/AutoPanoSift/AutopanoExe"),MY_G_STR_VAL("prefs_AutoPanoSIFTExe"));
    cfg->Write(wxT("/AutoPanoSift/Args"),MY_G_STR_VAL("prefs_AutoPanoSIFTArgs"));

    cfg->Write(wxT("/AutoPanoKolor/AutopanoExe"),MY_G_STR_VAL("prefs_AutoPanoKolorExe"));
    cfg->Write(wxT("/AutoPanoKolor/Args"),MY_G_STR_VAL("prefs_AutoPanoKolorArgs"));


    /////
    /// ENBLEND
    cfg->Write(wxT("/Enblend/EnblendExe"), MY_G_STR_VAL("prefs_enblend_EnblendExe"));
    cfg->Write(wxT("/Enblend/EnblendArgs"), MY_G_STR_VAL("prefs_enblend_EnblendArgs"));
    cfg->Write(wxT("/Enblend/DeleteRemappedFiles"), MY_G_BOOL_VAL("prefs_enblend_DeleteRemapped"));

}
