// SaveToFile.cpp: implementation of the CSaveToFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ipscan.h"
#include "IpscanDlg.h"
#include "SaveToFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSaveToFile::CSaveToFile(CIpscanDlg *dlg, BOOL bSaveSelection, LPSTR szFileName, BOOL bCSV, BOOL bAppend)
{
	m_dlg = dlg;

	m_bAppend = bAppend;
	
	if (dlg->m_list.GetItemCount() == 0) 
	{
		dlg->MessageBox("Nothing to save!",NULL,MB_OK | MB_ICONHAND);
		return;
	}

	m_saveselection = bSaveSelection;
	if (bSaveSelection && dlg->m_list.GetSelectedCount()==0)
	{		
		dlg->MessageBox("You must select items first",NULL,MB_OK | MB_ICONHAND);
		return;
	}
	
	BOOL bResult;
	
	if (szFileName == NULL) 
	{
		bResult = queryFilename();
	}
	else
	{
		bResult = TRUE;
		m_filename = szFileName;
	}

	if (bCSV) m_filetype = FILE_TYPE_CSV;
	
	if (bResult)
	{
		if (m_filetype == FILE_TYPE_CSV)
		{
			saveToCSV();
		}
		else
		{
			saveToTXT();
		}
	}
		
}

CSaveToFile::~CSaveToFile()
{
	
}

void CSaveToFile::saveToTXT()
{
	FILE *f;
	if (m_bAppend)
		f = fopen(m_filename,"at");
	else
		f = fopen(m_filename,"wt");
	
	if (!f) 
	{
		m_dlg->MessageBox("Cannot open file for writing",NULL,MB_OK | MB_ICONHAND);
		return;
	}

	CString szHomepage;
	szHomepage.LoadString(IDS_HOMEPAGE);

	if (!m_bAppend)
		fprintf(f,"This file was generated by Angry IP Scanner\n"
			  "Visit " + szHomepage + " for the latest version\n\n");
	
	int i,j;
	int ws[C_COLUMNS];

	CString tmp;
	LV_ITEM it;

	// Output column names
	
	for (i=0; i < C_COLUMNS; i++) 
	{
		ws[i] = m_dlg->m_list.GetColumnWidth(i) / 8 + 10;
		
		if (!m_bAppend)
		{
			tmp.LoadString(IDS_FIRSTCOLUMN+i*2+1);
			fprintf(f,"%-*s",ws[i],tmp);
		}
	}

	if (!m_bAppend)
		fputs("\n\n", f);
	

	// Output data
	if (m_saveselection)
	{
		for (i=0; i < m_dlg->m_list.GetItemCount(); i++) 
		{
			it.mask = LVIF_STATE; 
			it.stateMask = LVIS_SELECTED;
			it.iItem = i; 
			it.iSubItem = CL_IP; 

			m_dlg->m_list.GetItem(&it); 
			if (it.state != LVIS_SELECTED) continue;

			for (j=0; j<C_COLUMNS; j++) 
			{
				fprintf(f, "%-*s",ws[j], m_dlg->m_list.GetItemText(i,j));
			}
			fputs("\n", f);
		}
	}
	else  // save all, not only selected items
	{
		for (i=0; i < m_dlg->m_list.GetItemCount(); i++) 
		{					
			for (j=0; j<C_COLUMNS; j++) 
			{
				fprintf(f, "%-*s",ws[j], m_dlg->m_list.GetItemText(i,j));
			}
			fputs("\n", f);
		}
	}

	fclose(f);
}

void CSaveToFile::saveToCSV()
{
	FILE *f;
	if (m_bAppend)
		f = fopen(m_filename,"at");
	else
		f = fopen(m_filename,"wt");
	
	if (!f) 
	{
		m_dlg->MessageBox("Cannot open file for writing",NULL,MB_OK | MB_ICONHAND);
		return;
	}	
	
	int i,j;	

	CString tmp;	
	LV_ITEM it;

	// Output data
	if (m_saveselection)
	{
		for (i=0; i < m_dlg->m_list.GetItemCount(); i++) 
		{
			it.mask = LVIF_STATE; 
			it.stateMask = LVIS_SELECTED;
			it.iItem = i; 
			it.iSubItem = CL_IP; 

			m_dlg->m_list.GetItem(&it); 
			if (it.state != LVIS_SELECTED) continue;

			for (j=0; j < C_COLUMNS; j++) 
			{
				fprintf(f, "%s", m_dlg->m_list.GetItemText(i,j));
				if (j < C_COLUMNS-1) fputs(",", f);
			}
			fputs("\n", f);
		}
	}
	else  // save all, not only selected items
	{		
		for (i=0; i < m_dlg->m_list.GetItemCount(); i++) 
		{
			for (j=0; j < C_COLUMNS; j++) 
			{
				fprintf(f, "%s", m_dlg->m_list.GetItemText(i,j));
				if (j < C_COLUMNS-1) fputs(",", f);
			}
			fputs("\n", f);
		}
	}

	fclose(f);
}

BOOL CSaveToFile::queryFilename()
{
	CFileDialog fd(FALSE,"txt",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Text files (*.txt)|*.txt|Comma-separated files (*.csv)|*.csv||", m_dlg);

	if (fd.DoModal() == IDOK)
	{		

		if (fd.GetFileExt().CompareNoCase("csv") == 0)
		{
			m_filetype = FILE_TYPE_CSV;
		}
		else
		{
			m_filetype = FILE_TYPE_TXT;

		}

		m_filename = fd.GetPathName();

		return TRUE;
	}
	else
	{
		return FALSE;
	}	

}
