// File: treedraw.cc -- contains the configuration class for the extensive
// form
// @(#)treedraw.cc	1.5 7/18/95
#include "zfortify.hpp"
#include "wx.h"
#include "wx_form.h"
#pragma hdrstop
#include "wxmisc.h"
#include "treedraw.h"
#include "legendc.h"

TreeDrawSettings::TreeDrawSettings(void)
{ 
zoom_factor=1.0;
wxPathList path_list;
path_list.AddEnvList("PATH");
char *file_name=path_list.FindValidPath("gambit.ini");
if (!file_name)
{
	// if there is no ini file yet, use the hard coded defaults and create
	// the ini file from them.
	flashing_cursor=FLASHING_CURSOR_DEFAULT;
	branch_length=BRANCH_LENGTH_DEFAULT;
	node_length=NODE_LENGTH_DEFAULT;
	outcome_length=OUTCOME_LENGTH_DEFAULT;
	y_spacing=Y_SPACING_DEFAULT;
	x_origin=0;y_origin=0;
	chance_color=CHANCE_COLOR_DEFAULT;
	cursor_color=CURSOR_COLOR_DEFAULT;
	show_infosets=SHOW_INFOSETS_DEFAULT;
	node_above_label=NODE_ABOVE_LABEL;
	node_below_label=NODE_BELOW_INFOSET;
	branch_above_label=BRANCH_ABOVE_LABEL;
	branch_below_label=BRANCH_BELOW_PROBS;
	node_terminal_label=NODE_TERMINAL_OUTCOME;
	node_above_font=new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);
	node_below_font=new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);
	branch_above_font=new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);
	branch_below_font=new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);
	node_terminal_font=new wxFont(12,wxSWISS,wxNORMAL,wxNORMAL);
	SaveOptions();
}
else
{
	// if the ini file exists, read the defaults from it
	x_origin=0;y_origin=0;
	node_above_font=NULL;
	node_below_font=NULL;
	branch_above_font=NULL;
	branch_below_font=NULL;
	node_terminal_font=NULL;

	LoadOptions(file_name);
}
}


void TreeDrawSettings::SetOptions(void)
{
wxFont fixed_font(12,wxMODERN,wxNORMAL,wxNORMAL);
MyDialogBox *tree_options_dialog=new MyDialogBox(NULL,"Draw Options");
tree_options_dialog->SetLabelFont(&fixed_font);
tree_options_dialog->Form()->Add(wxMakeFormMessage("Size Settings"));
tree_options_dialog->Form()->Add(wxMakeFormNewLine());
tree_options_dialog->Form()->Add(wxMakeFormShort("Branch Length",&branch_length,wxFORM_SLIDER,
											 new wxList(wxMakeConstraintRange(BRANCH_LENGTH_MIN, BRANCH_LENGTH_MAX),0),
											 NULL,wxHORIZONTAL,150));
tree_options_dialog->Form()->Add(wxMakeFormNewLine());
tree_options_dialog->Form()->Add(wxMakeFormShort("Node Length  ",&node_length,wxFORM_SLIDER,
											 new wxList(wxMakeConstraintRange(NODE_LENGTH_MIN,NODE_LENGTH_MAX), 0),
											 NULL,wxHORIZONTAL,150));
tree_options_dialog->Form()->Add(wxMakeFormNewLine());
tree_options_dialog->Form()->Add(wxMakeFormShort("Y Spacing    ",&y_spacing,wxFORM_SLIDER,
											 new wxList(wxMakeConstraintRange(Y_SPACING_MIN, Y_SPACING_MAX), 0),
											 NULL,wxHORIZONTAL,150));
tree_options_dialog->Form()->Add(wxMakeFormNewLine());
tree_options_dialog->Form()->Add(wxMakeFormBool("Show Infosets",&show_infosets,wxFORM_CHECKBOX));
tree_options_dialog->Form()->Add(wxMakeFormBool("Flashing Cursor",&flashing_cursor,wxFORM_CHECKBOX));
tree_options_dialog->Form()->Add(wxMakeFormNewLine());
tree_options_dialog->Go();
}

// SetLegends
// Selects what get displayed at different points on the tree and in what
// font it will be displayed
#define	NAF	10
#define	NBF	11
#define	BAF	12
#define	BBF	13
#define	NTF	14
#pragma argsused		// turn off the ev not used message
void TreeDrawSettings::draw_params_legends_func(wxButton &ob,wxCommandEvent &ev)
{
FontDialogBox *f;
draw_params_legend_struct *dpls=(draw_params_legend_struct *)ob.GetClientData();
// The following case makes sure that the currently set font comes up first
switch (dpls->what_font)
{
	case NAF: f=new FontDialogBox((wxWindow *)ob.GetParent(),dpls->draw_settings->NodeAboveFont());break;
	case NBF: f=new FontDialogBox((wxWindow *)ob.GetParent(),dpls->draw_settings->NodeBelowFont());break;
	case BAF: f=new FontDialogBox((wxWindow *)ob.GetParent(),dpls->draw_settings->BranchAboveFont());break;
	case BBF: f=new FontDialogBox((wxWindow *)ob.GetParent(),dpls->draw_settings->BranchBelowFont());break;
	case NTF: f=new FontDialogBox((wxWindow *)ob.GetParent(),dpls->draw_settings->NodeTerminalFont());break;
}

if (f->Completed())
{
	wxFont *the_font=f->MakeFont();
	switch (dpls->what_font)
	{
		case NAF: dpls->draw_settings->SetNodeAboveFont(the_font);break;
		case NBF: dpls->draw_settings->SetNodeBelowFont(the_font);break;
		case BAF: dpls->draw_settings->SetBranchAboveFont(the_font);break;
		case BBF: dpls->draw_settings->SetBranchBelowFont(the_font);break;
		case NTF: dpls->draw_settings->SetNodeTerminalFont(the_font);break;
	}
}
delete f;
}

void TreeDrawSettings::SetLegends(void)
{
#include "legend.h"
int i;
wxStringList 	*node_above_list=new wxStringList;
wxStringList 	*node_below_list=new wxStringList;
wxStringList 	*branch_above_list=new wxStringList;
wxStringList 	*branch_below_list=new wxStringList;
wxStringList	*node_terminal_list=new wxStringList;
char					*node_above_str=new char[20];
char					*node_below_str=new char[20];
char					*branch_above_str=new char[20];
char					*branch_below_str=new char[20];
char					*node_terminal_str=new char[20];
int						node_above_num;
int						node_below_num;
int						branch_above_num;
int						branch_below_num;
int						node_terminal_num;
wxFont				fixed_font(12,wxMODERN,wxNORMAL,wxNORMAL);

// Create the appropriate lists from the "legends.h" file
i=0;
while (node_above_src[i].l_id!=-1)
	{node_above_list->Add(node_above_src[i].l_name); i++;}
i=0;
while (node_below_src[i].l_id!=-1)
	{node_below_list->Add(node_below_src[i].l_name); i++;}
i=0;
while (branch_above_src[i].l_id!=-1)
	{branch_above_list->Add(branch_above_src[i].l_name); i++;}
i=0;
while (branch_below_src[i].l_id!=-1)
	{branch_below_list->Add(branch_below_src[i].l_name); i++;}
i=0;
while (node_terminal_src[i].l_id!=-1)
	{node_terminal_list->Add(node_terminal_src[i].l_name); i++;}
// Copy the current settings into the new dialog
strcpy(node_above_str,node_above_src[LabelNodeAbove()].l_name);
strcpy(node_below_str,node_below_src[LabelNodeBelow()].l_name);
strcpy(branch_above_str,branch_above_src[LabelBranchAbove()].l_name);
strcpy(branch_below_str,branch_below_src[LabelBranchBelow()].l_name);
strcpy(node_terminal_str,node_terminal_src[LabelNodeTerminal()].l_name);
// Create the dialog box
MyDialogBox *display_legend_dialog=new MyDialogBox(NULL,"Display Legends");
display_legend_dialog->SetLabelFont(&fixed_font);
display_legend_dialog->Form()->Add(wxMakeFormMessage("                   What to Display"));
display_legend_dialog->Form()->Add(wxMakeFormNewLine());
display_legend_dialog->Form()->Add(wxMakeFormMessage("Above"));
wxFormItem *node_above_button=wxMakeFormButton("Font",(wxFunction)draw_params_legends_func);
display_legend_dialog->Form()->Add(node_above_button);

display_legend_dialog->Form()->Add(wxMakeFormString(NULL,&node_above_str,wxFORM_CHOICE,
				new wxList(wxMakeConstraintStrings(node_above_list),0)));
display_legend_dialog->Form()->Add(wxMakeFormString(NULL,&branch_above_str,wxFORM_CHOICE,
				new wxList(wxMakeConstraintStrings(branch_above_list),0)));
wxFormItem *branch_above_button=wxMakeFormButton("Font",(wxFunction)draw_params_legends_func);
display_legend_dialog->Form()->Add(branch_above_button);

display_legend_dialog->Form()->Add(wxMakeFormNewLine());
display_legend_dialog->Form()->Add(wxMakeFormMessage("              Node           "));
display_legend_dialog->Form()->Add(wxMakeFormMessage("Branch"));
display_legend_dialog->Form()->Add(wxMakeFormNewLine());
display_legend_dialog->Form()->Add(wxMakeFormMessage("Below"));
wxFormItem *node_below_button=wxMakeFormButton("Font",(wxFunction)draw_params_legends_func);
display_legend_dialog->Form()->Add(node_below_button);

display_legend_dialog->Form()->Add(wxMakeFormString(NULL,&node_below_str,wxFORM_CHOICE,
				new wxList(wxMakeConstraintStrings(node_below_list),0)));
display_legend_dialog->Form()->Add(wxMakeFormString(NULL,&branch_below_str,wxFORM_CHOICE,
				new wxList(wxMakeConstraintStrings(branch_below_list),0)));
wxFormItem *branch_below_button=wxMakeFormButton("Font",(wxFunction)draw_params_legends_func);
display_legend_dialog->Form()->Add(branch_below_button);
display_legend_dialog->Form()->Add(wxMakeFormNewLine());
display_legend_dialog->Form()->Add(wxMakeFormNewLine());
display_legend_dialog->Form()->Add(wxMakeFormString("At terminal",&node_terminal_str,wxFORM_CHOICE,
				new wxList(wxMakeConstraintStrings(node_terminal_list),0)));
wxFormItem *node_terminal_button=wxMakeFormButton("Font",(wxFunction)draw_params_legends_func);
display_legend_dialog->Form()->Add(node_terminal_button);

display_legend_dialog->Form()->AssociatePanel(display_legend_dialog);
display_legend_dialog->Fit();
// Setup the font buttons
draw_params_legend_struct dpls1={NAF,this};
node_above_button->PanelItem->SetClientData((char *)&dpls1);
draw_params_legend_struct dpls2={NBF,this};
node_below_button->PanelItem->SetClientData((char *)&dpls2);
draw_params_legend_struct dpls3={BAF,this};
branch_above_button->PanelItem->SetClientData((char *)&dpls3);
draw_params_legend_struct dpls4={BBF,this};
branch_below_button->PanelItem->SetClientData((char *)&dpls4);
draw_params_legend_struct dpls5={NTF,this};
node_terminal_button->PanelItem->SetClientData((char *)&dpls5);
// Start the dialog
display_legend_dialog->Show(TRUE);
// Process the output of the dialog
if (display_legend_dialog->Completed())
{
	node_above_num=wxListFindString(node_above_list,node_above_str);
	node_below_num=wxListFindString(node_below_list,node_below_str);
	branch_above_num=wxListFindString(branch_above_list,branch_above_str);
	branch_below_num=wxListFindString(branch_below_list,branch_below_str);
  node_terminal_num=wxListFindString(node_terminal_list,node_terminal_str);
	SetLabelNodeAbove(node_above_src[node_above_num].l_id);
	SetLabelNodeBelow(node_below_src[node_below_num].l_id);
	SetLabelBranchAbove(branch_above_src[branch_above_num].l_id);
	SetLabelBranchBelow(branch_below_src[branch_below_num].l_id);
	SetLabelNodeTerminal(node_terminal_src[node_terminal_num].l_id);
}

// Clean up...
delete [] node_above_str;
delete [] node_below_str;
delete [] branch_above_str;
delete [] branch_below_str;
delete [] node_terminal_str;
delete display_legend_dialog;
}

// Save options.  Uses the resource writing capability of wxwin to create
// the file "gambit.ini" to which to write all the current settings from
// draw_settings.  The file is ASCII and can be edited by hand if necessary.
void TreeDrawSettings::SaveOptions(char *s)
{
char *file_name;

file_name=copystring((s) ? s : "gambit.ini");

wxWriteResource("Gambit","Branch-Length",branch_length,file_name);
wxWriteResource("Gambit","Node-Length",node_length,file_name);
wxWriteResource("Gambit","Outcome-Length",outcome_length,file_name);
wxWriteResource("Gambit","Y-Spacing",y_spacing,file_name);
wxWriteResource("Gambit","Chance-Color",chance_color,file_name);
wxWriteResource("Gambit","Cursor-Color",cursor_color,file_name);
wxWriteResource("Gambit","Show-Infosets",show_infosets,file_name);
wxWriteResource("Gambit","Node-Above-Label",node_above_label,file_name);
wxWriteResource("Gambit","Node-Below-Label",node_below_label,file_name);
wxWriteResource("Gambit","Branch-Above-Label",branch_above_label,file_name);
wxWriteResource("Gambit","Branch-Below-Label",branch_below_label,file_name);
wxWriteResource("Gambit","Node-Terminal-Label",node_terminal_label,file_name);
wxWriteResource("Gambit","Flashing-Cursor",flashing_cursor,file_name);
// Save the font settings
wxWriteResource("Gambit","Node-Above-Font",wxFontToString(node_above_font),file_name);
wxWriteResource("Gambit","Node-Below-Font",wxFontToString(node_below_font),file_name);
wxWriteResource("Gambit","Branch-Above-Font",wxFontToString(branch_above_font),file_name);
wxWriteResource("Gambit","Branch-Below-Font",wxFontToString(branch_below_font),file_name);
wxWriteResource("Gambit","Node-Terminal-Font",wxFontToString(node_terminal_font),file_name);
GambitDrawSettings::SaveOptions(file_name);
delete [] file_name;
}
// Load options.  Uses the resource writing capability of wxwin to read from
// the file file_name all the current settings for the
// draw_settings.  The file is ASCII and can be edited by hand if necessary.
void TreeDrawSettings::LoadOptions(char *file_name)
{
if (!file_name)
{
	wxPathList path_list;
	path_list.AddEnvList("PATH");
	file_name=path_list.FindValidPath("gambit.ini");
}

if (!FileExists(file_name))
{
	wxMessageBox("Configuration file not found","Error",wxOK | wxCENTRE);
	return;
}
file_name=copystring(file_name);	// must do this: file_name was temp
wxGetResource("Gambit","Branch-Length",&branch_length,file_name);
wxGetResource("Gambit","Node-Length",&node_length,file_name);
wxGetResource("Gambit","Outcome-Length",&outcome_length,file_name);
wxGetResource("Gambit","Y-Spacing",&y_spacing,file_name);
wxGetResource("Gambit","Chance-Color",&chance_color,file_name);
wxGetResource("Gambit","Cursor-Color",&cursor_color,file_name);
wxGetResource("Gambit","Show-Infosets",&show_infosets,file_name);
wxGetResource("Gambit","Node-Above-Label",&node_above_label,file_name);
wxGetResource("Gambit","Node-Below-Label",&node_below_label,file_name);
wxGetResource("Gambit","Branch-Above-Label",&branch_above_label,file_name);
wxGetResource("Gambit","Branch-Below-Label",&branch_below_label,file_name);
wxGetResource("Gambit","Node-Terminal-Label",&node_terminal_label,file_name);
wxGetResource("Gambit","Flashing-Cursor",&flashing_cursor,file_name);
// Load the font settings
char *l_tempstr=new char [100];
wxGetResource("Gambit","Node-Above-Font",&l_tempstr,file_name);
SetNodeAboveFont(wxStringToFont(l_tempstr));
wxGetResource("Gambit","Node-Below-Font",&l_tempstr,file_name);
SetNodeBelowFont(wxStringToFont(l_tempstr));
wxGetResource("Gambit","Branch-Above-Font",&l_tempstr,file_name);
SetBranchAboveFont(wxStringToFont(l_tempstr));
wxGetResource("Gambit","Branch-Below-Font",&l_tempstr,file_name);
SetBranchBelowFont(wxStringToFont(l_tempstr));
wxGetResource("Gambit","Node-Terminal-Font",&l_tempstr,file_name);
SetNodeTerminalFont(wxStringToFont(l_tempstr));
GambitDrawSettings::LoadOptions(file_name);
delete [] file_name;
}

//********************************* SET ZOOM *******************************
// set force to true to set a zoom that is outside the MIN_ZOOM,MAX_ZOOM range
void TreeDrawSettings::SetZoom(float z,bool force)
{
#define MAX_ZOOM	10
#define MIN_ZOOM	.1
if (z<-.5)			// if this is not an inc/dec action
{
	float t_zoom_factor=zoom_factor;
	MyDialogBox *zoom_dialog = new MyDialogBox(NULL, "Set Zoom");
	zoom_dialog->Form()->Add(wxMakeFormFloat("Zoom [0.1-10]", &t_zoom_factor, wxFORM_DEFAULT,
							 new wxList(wxMakeConstraintRange(MIN_ZOOM,MAX_ZOOM), 0)));
	zoom_dialog->Go();
	if (zoom_dialog->Completed()==wxOK)
		zoom_factor=t_zoom_factor;
	delete zoom_dialog;
}
else
{
	if (z>=MIN_ZOOM && z<=MAX_ZOOM || force)	zoom_factor=z;
}
}
