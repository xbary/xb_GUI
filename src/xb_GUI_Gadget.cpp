#include <xb_board.h>

#ifdef XB_GUI
#include "xb_GUI_Gadget.h"

#ifdef ARDUINO_ARCH_STM32F1
extern "C" {
#include <string.h>
#include <stdlib.h>

	extern void *malloc(size_t size);
	extern void free(void *memblock);
	extern size_t strlen(const char *str);
	extern void *memcpy(void *dest, const void *src, size_t count);
	extern int sprintf(char *buffer, const char *format, ...);
	extern char *strdup(const char *);
}
#endif
#include <IPAddress.h>


TTaskDef XB_GUIGADGET_DefTask = { 3,&GUIGADGET_Setup,&GUIGADGET_Loop,&GUIGADGET_Message};

typedef enum { dgHandle } TGUIGADGETFunction;
TGUIGADGETFunction GUIGADGETFunction = dgHandle;

DEFLIST_VAR(TGADGETMenu,GADGETMenusList)
DEFLIST_VAR(TGADGETInputDialog,GADGETInputDialogsList)

void GUIGADGET_Setup(void)
{
	GADGETMenusList = NULL;
	GADGETInputDialogsList = NULL;

	GUIGADGETFunction = dgHandle;
}

uint32_t GUIGADGET_Loop(void)
{
	switch (GUIGADGETFunction)
	{
	case dgHandle:
	{
		break;
	}
	default: break;
	}
	return 0;
}

bool GUIGADGET_Message(TMessageBoard *Am)
{
	bool res = false;
	
	switch (Am->IDMessage)
	{
	case IM_HANDLEPTR:
	{
		TGADGETInputDialog* gid = GADGETInputDialogsList;
		while (gid != NULL)
		{
			HANDLEPTR(gid->WindowClass);
			gid = gid->Next;
		}

		TGADGETMenu* gm = GADGETMenusList;
		while (gm != NULL)
		{
			HANDLEPTR(gm->WindowClass);
			gm = gm->Next;
		}

		res = true;
		break;
	}
#ifdef XB_GUI
	case IM_WINDOW:
	{
		switch (Am->Data.WindowData.WindowAction)
		{
		case waCreate:
		{
			TGADGETMenu *m = GUIGADGET_FindMenuByIDWindow(Am->Data.WindowData.ID);
			if (m != NULL)
			{
				int16_t xt = (GUI_GetWidthDesktop() / 2) - (m->WidthItems / 2);
				if (xt < 0) xt = 0;
				int16_t yt = (GUI_GetHeightDesktop() / 2) - ((m->ItemCount + 2) / 2);
				if (yt < 0) yt = 0;
				Am->Data.WindowData.ActionData.Create.X = WINDOW_POS_X_DEF;
				Am->Data.WindowData.ActionData.Create.Y = WINDOW_POS_Y_DEF;
				Am->Data.WindowData.ActionData.Create.Width = m->WidthItems + 2;
				Am->Data.WindowData.ActionData.Create.Height = m->ItemCount + 2;
				res = true;
				break;
			}

			TGADGETInputDialog *id = GUIGADGET_FindInputDialogByIDWindow(Am->Data.WindowData.ID);
			if (id != NULL)
			{

				String str;
				id->GetDescriptionInputDialogString(str);
				int16_t w = str.length()+4, h = 5;
				if (id->MaxLengthVar + 6 > w ) w = id->MaxLengthVar + 6;
				
				

				int16_t xt = (GUI_GetWidthDesktop() / 2) - (w / 2);
				if (xt < 0) xt = 0;
				int16_t yt = (GUI_GetHeightDesktop() / 2) - ((h + 2) / 2);
				if (yt < 0) yt = 0;
				Am->Data.WindowData.ActionData.Create.X = WINDOW_POS_X_DEF;
				Am->Data.WindowData.ActionData.Create.Y = WINDOW_POS_Y_DEF;
				Am->Data.WindowData.ActionData.Create.Width = w + 2;
				Am->Data.WindowData.ActionData.Create.Height = h + 2;
				res = true;
				break;
			}
			
			break;
		}
		case waDestroy:
		{
			TGADGETMenu *m = GUIGADGET_FindMenuByIDWindow(Am->Data.WindowData.ID);
			if (m != NULL)
			{
				// oznajmienie zadania �e jego okno zosta�o skasowane
				GUIGADGET_DestroyMenu(&m);
				res = true;
				break;
			}

			TGADGETInputDialog *id = GUIGADGET_FindInputDialogByIDWindow(Am->Data.WindowData.ID);
			if (id != NULL)
			{
				GUIGADGET_DestroyInputDialog(&id);
				res = true;
				break;
			}

			break;
		}
		case waGetCaptionWindow:
		{
			TGADGETMenu *m = GUIGADGET_FindMenuByIDWindow(Am->Data.WindowData.ID);
			if (m != NULL)
			{
				m->GetCaptionMenuString(*(Am->Data.WindowData.ActionData.GetCaption.PointerString));
				res = true;
				break;
			}

			TGADGETInputDialog *id = GUIGADGET_FindInputDialogByIDWindow(Am->Data.WindowData.ID);
			if (id != NULL)
			{
				id->GetCaptionInputDialogString(*(Am->Data.WindowData.ActionData.GetCaption.PointerString));
				res = true;
				break;
			}

			break;
		}
		case waRepaint:
		{
			TGADGETMenu *m = GUIGADGET_FindMenuByIDWindow(Am->Data.WindowData.ID);
			if (m != NULL)
			{
				if (m->WindowClass != NULL)
				{
					m->WindowClass->RepaintDataCounter++;
					m->TypePaintMenuGadget = tpmgAll;
				}
				res = true;
				break;
			}

			TGADGETInputDialog *id = GUIGADGET_FindInputDialogByIDWindow(Am->Data.WindowData.ID);
			if (id != NULL)
			{
				if (id->WindowClass != NULL)
				{
					String str;
					id->GetDescriptionInputDialogString(str);
					id->WindowClass->BeginDraw();
					{
						id->WindowClass->SetTextColor(tfcYellow);
						id->WindowClass->PutStr(1, 1, str.c_str());
					}
					id->WindowClass->EndDraw();
				}
				res = true;
				break;
			}
			break;
		}
		case waRepaintData:
		{
			TGADGETMenu *m = GUIGADGET_FindMenuByIDWindow(Am->Data.WindowData.ID);
			if (m != NULL)
			{
				if (m->WindowClass != NULL)
				{
					m->WindowClass->BeginDraw();
					{
						m->WindowClass->SetTextColor(tfcWhite);
						m->WindowClass->SetTextBackgoundColor(tbcBlack);
						m->WindowClass->SetNormalChar();

						String str;
						str.reserve(m->WindowClass->Width + 4);

						switch (m->TypePaintMenuGadget)
						{
						case tpmgAll:
							{
								for (uint8_t i = 0; i < m->ItemCount; i++)
								{
									str = "";
									TTextAlignment textalignment = taLeft; 
									bool Nmenu = false;
									if (m->GetItemMenuString(i, str,&textalignment,&Nmenu))
									{
										if (str == "-")
										{
											for (uint8_t i = 0; i < m->WindowClass->Width - 3; i++) str = str + '-';
										}

										if (m->CurrentItem == i)
										{
											m->WindowClass->SetReverseChar();
										}

										if (Nmenu == false)
										{
											m->WindowClass->PutStr(0, i, str.c_str(), m->WidthItems, ' ', textalignment);
										}
										else
										{
											m->WindowClass->PutStr(0, i, str.c_str(), m->WidthItems-3, ' ', textalignment);
											m->WindowClass->PutStr(m->WidthItems - 3, i, " >>");
										}

										if (m->CurrentItem == i)
										{
											m->WindowClass->SetNormalChar();
										}

										

									}
								}
								m->TypePaintMenuGadget = tpmgNone;
								break;
						
							}
						case tpmgOnlyLastCurrent:
							{
								str="";
								TTextAlignment textalignment = taLeft; 
								bool Nmenu = false;
								m->GetItemMenuString(m->LastItem, str, &textalignment, &Nmenu);
								if (str == "-")	for (uint8_t i = 0; i < m->WindowClass->Width - 3; i++) str = str + '-';
								if (Nmenu == false)
								{
									m->WindowClass->PutStr(0, m->LastItem, str.c_str(), m->WidthItems, ' ', textalignment);
								}
								else
								{
									m->WindowClass->PutStr(0, m->LastItem, str.c_str(), m->WidthItems-3, ' ', textalignment);
									m->WindowClass->PutStr(m->WidthItems - 3, m->LastItem, " >>");
								}

								m->WindowClass->SetReverseChar();
								textalignment = taLeft; 
								m->GetItemMenuString(m->CurrentItem, str, &textalignment,&Nmenu);
								if (Nmenu == false)
								{
									m->WindowClass->PutStr(0, m->CurrentItem, str.c_str(), m->WidthItems, ' ', textalignment);
								}
								else
								{
									m->WindowClass->PutStr(0, m->CurrentItem, str.c_str(), m->WidthItems-3, ' ', textalignment);
									m->WindowClass->PutStr(m->WidthItems - 3, m->CurrentItem, " >>");

								}
								m->TypePaintMenuGadget = tpmgNone;
								break;
							}
						case tpmgOnlyCurrent:
							{
								str = "";
								TTextAlignment textalignment = taLeft; 
								bool Nmenu = false;
								m->GetItemMenuString(m->CurrentItem, str, &textalignment, &Nmenu);
								if (str == "-")	for (uint8_t i = 0; i < m->WindowClass->Width - 3; i++) str = str + '-';
								m->WindowClass->SetReverseChar();
								if (Nmenu == false)
								{
									m->WindowClass->PutStr(0, m->CurrentItem, str.c_str(), m->WidthItems, ' ', textalignment);
								}
								else
								{
									m->WindowClass->PutStr(0, m->CurrentItem, str.c_str(), m->WidthItems-3, ' ', textalignment);
									m->WindowClass->PutStr(m->WidthItems - 3, m->CurrentItem, " >>");
								}

								m->TypePaintMenuGadget = tpmgNone;
								break;
							}
						default: break;
						}
					}
					m->WindowClass->EndDraw();
				}
				res = true;
				break;
			}

			TGADGETInputDialog *id = GUIGADGET_FindInputDialogByIDWindow(Am->Data.WindowData.ID);
			if (id != NULL)
			{
				id->PaintVar();
				res = true;
				break;
			}

			break;
		
		}
		default: break;
		}
		break;
	}
#endif
	case IM_KEYBOARD:
	{
#ifdef XB_GUI
		
		if (Am->Data.KeyboardData.TypeKeyboardAction == tkaKEYPRESS)
		{
			TWindowClass *w = GUI_FindWindowByActive();
			if (w == NULL) break;
			TGADGETMenu *m = GUIGADGET_FindMenuByWindowClass(w);
			if (m != NULL)
			{

				switch (Am->Data.KeyboardData.KeyFunction)
				{
				case KF_CURSORUP:
				{
					if (m->CurrentItem != 0)
					{
						String str="";
						m->GetItemMenuString(m->CurrentItem - 1, str);
						if (str == "-")
						{
							if ((m->CurrentItem - 1) != 0)
							{
								m->ChangeCurrentItemMenu(m->CurrentItem - 2);
							}
						}
						else
						{
							m->ChangeCurrentItemMenu(m->CurrentItem - 1);
						}
						
						res = true;
					}
					break;
				}
				case KF_CURSORDOWN:
				{
					if (m->CurrentItem < m->ItemCount - 1)
					{
						String str="";
						m->GetItemMenuString(m->CurrentItem + 1, str);
						if (str == "-")
						{
							if (m->CurrentItem+1 < m->ItemCount - 1)
							{
								m->ChangeCurrentItemMenu(m->CurrentItem + 2);
							}
						}
						else
						{
							m->ChangeCurrentItemMenu(m->CurrentItem + 1);	
						}
						
						res = true;
					}
					break;
				}
				case KF_ESC:
				{
					if (m->SendEscapeMessageToOwnerTask())
					{
						if (m->WindowClass != NULL)
						{
							GUI_WindowDestroy(&m->WindowClass);
							GUI_ClearDesktop();
							GUI_RepaintAllWindows();
						}
					}
					res = true;
					break;
				}
				case KF_INSERT:
				{
					if (m->SendInsertMessageToOwnerTask())
					{
					}
					res = true;
					break;
				}
				case KF_ENTER:
				{
			
					Am->Data.KeyboardData.KeyFunction = KF_NONE;
					m->ClickItemMenu(m->CurrentItem);
					
					res = true;
					break;
				}
				case KF_CURSORLEFT:
				{

					Am->Data.KeyboardData.KeyFunction = KF_NONE;
					m->ClickLeftItemMenu(m->CurrentItem);

					res = true;
					break;
				}
				case KF_CURSORRIGHT:
				{

					Am->Data.KeyboardData.KeyFunction = KF_NONE;
					m->ClickRightItemMenu(m->CurrentItem);

					res = true;
					break;
				}
				case KF_DELETE:
				{
			
					Am->Data.KeyboardData.KeyFunction = KF_NONE;
					m->DeleteItemMenu(m->CurrentItem);
					
					res = true;
					break;
				}
				default:break;
				}
				break;
			}

			TGADGETInputDialog *id = GUIGADGET_FindInputDialogByWindowClass(w);
			if (id != NULL)
			{
				if (id->WindowClass != NULL)
				{
					switch (Am->Data.KeyboardData.KeyFunction)
					{
					case KF_CURSORLEFT:
					{
						id->MoveEditCursor(-1);
						id->PaintVar();
						res = true;
						break;
					}
					case KF_CURSORRIGHT:
					{
						id->MoveEditCursor(1);
						id->PaintVar();
						res = true;
						break;
					}
					case KF_BACKSPACE:
					{
						id->MoveEditCursor(-1);
						id->EditVar.remove(id->CursorPosInputVar, 1);
						id->SendChangeValueMessageToOwnerTask();
						id->PaintVar();
						res = true;
						break;
					}
					case KF_DELETE:
					{
						id->EditVar.remove(id->CursorPosInputVar, 1);
						id->SendChangeValueMessageToOwnerTask();
						id->PaintVar();
						res = true;
						break;
					}
					case KF_ENTER:
					{
						Am->Data.KeyboardData.KeyFunction = KF_NONE;
						id->EnterVAR();
						id->SendEnterMessageToOwnerTask();
						GUI_WindowDestroy(&id->WindowClass);
						GUI_ClearDesktop();
						res = true;
						break;
					}
					case KF_ESC:
					{
						Am->Data.KeyboardData.KeyFunction = KF_NONE;
						id->SendEscapeMessageToOwnerTask();
						GUI_WindowDestroy(&id->WindowClass);
						GUI_ClearDesktop();
						res = true;
						break;
					}
					case KF_CODE:
					{
						char ch = 0;

						if ((id->TypeInputVar == tivDynArrayChar1) || (id->TypeInputVar == tivString))
						{
							if ((Am->Data.KeyboardData.KeyCode >= 32) && (Am->Data.KeyboardData.KeyCode <= 126))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
						}
						else if (id->TypeInputVar == tivDynArrayChar2)
						{
							if ((Am->Data.KeyboardData.KeyCode >= 33) && (Am->Data.KeyboardData.KeyCode <= 126))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
						}
						else if (id->TypeInputVar == tivDynArrayChar3)
						{
							if (((char)Am->Data.KeyboardData.KeyCode >= 'a') && ((char)Am->Data.KeyboardData.KeyCode <= 'z'))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
							else if (((char)Am->Data.KeyboardData.KeyCode >= 'A') && ((char)Am->Data.KeyboardData.KeyCode <= 'Z'))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
							else if (((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9'))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
						}
						else if ((id->TypeInputVar == tivIP) || (id->TypeInputVar == tivIP_U32))
						{
							if (((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9'))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
							else if ((char)Am->Data.KeyboardData.KeyCode == '.')
							{
								int c = 0;
								{
									int i = 0;
									while (i < id->EditVar.length())
									{
										if (id->EditVar.charAt(i) == '.') c++;
										i++;
									}
								}
								if (c < 3)
								{
									if (id->CursorPosInputVar > 0)
									{
										if (id->EditVar.charAt(id->CursorPosInputVar - 1) != '.') 
										{
											ch = (char)Am->Data.KeyboardData.KeyCode;		
										}
									}
								}
							}
						}
						else if ((id->TypeInputVar == tivUInt32) || (id->TypeInputVar == tivUInt16) || (id->TypeInputVar == tivUInt8))
						{
							if (((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9'))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
						}
						else if (id->TypeInputVar == tivUInt8_HEX)
						{
							if ((((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9')) || (((char)Am->Data.KeyboardData.KeyCode >= 'A') && ((char)Am->Data.KeyboardData.KeyCode <= 'F')) || (((char)Am->Data.KeyboardData.KeyCode >= 'a') && ((char)Am->Data.KeyboardData.KeyCode <= 'f')))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
						}
						else if (id->TypeInputVar == tiv_double)
						{
							if ((((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9')))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
							else if (((char)Am->Data.KeyboardData.KeyCode == '-'))
							{
								int c = 0;
								{
									int i = 0;
									while (i < id->EditVar.length())
									{
										if (id->EditVar.charAt(i) == '-') c++;
										i++;
									}
								}
								if (c == 0)
								{
									if (id->CursorPosInputVar == 0)
									{
										ch = (char)Am->Data.KeyboardData.KeyCode;
									}
								}
							}
							else if(((char)Am->Data.KeyboardData.KeyCode == '.'))
							{
								int c = 0;
								{
									int i = 0;
									while (i < id->EditVar.length())
									{
										if (id->EditVar.charAt(i) == '.') c++;
										i++;
									}
								}
								if (c == 0)
								{
									if (id->CursorPosInputVar > 0)
									{
										ch = (char)Am->Data.KeyboardData.KeyCode;
									}
								}
							}
						}
						else if (id->TypeInputVar == tivInt16)
						{
						if ((((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9')))
						{
							ch = (char)Am->Data.KeyboardData.KeyCode;
						}
						else if (((char)Am->Data.KeyboardData.KeyCode == '-'))
						{
							int c = 0;
							{
								int i = 0;
								while (i < id->EditVar.length())
								{
									if (id->EditVar.charAt(i) == '-') c++;
									i++;
								}
							}
							if (c == 0)
							{
								if (id->CursorPosInputVar == 0)
								{
									ch = (char)Am->Data.KeyboardData.KeyCode;
								}
							}
						}
						}
						else if (id->TypeInputVar == tiv_udouble)
						{
							if ((((char)Am->Data.KeyboardData.KeyCode >= '0') && ((char)Am->Data.KeyboardData.KeyCode <= '9')))
							{
								ch = (char)Am->Data.KeyboardData.KeyCode;
							}
							else if (((char)Am->Data.KeyboardData.KeyCode == '.'))
							{
								int c = 0;
								{
									int i = 0;
									while (i < id->EditVar.length())
									{
										if (id->EditVar.charAt(i) == '.') c++;
										i++;
									}
								}
								if (c == 0)
								{
									if (id->CursorPosInputVar > 0)
									{
										ch = (char)Am->Data.KeyboardData.KeyCode;
									}
								}
							}
						}

						if (ch != 0)
						{
							id->EditInsertChar(ch);
							id->SendChangeValueMessageToOwnerTask();
							id->PaintVar();
						}
						res = true;
						break;
					}
					default:break;
					}

				}
				break;
			}

		}
		break;
#endif
	}
	case IM_GET_TASKNAME_STRING:
	{
		*(Am->Data.PointerString) = FSS("GUIGADGET");
		res = true;
		break;
		
	}
	case IM_GET_TASKSTATUS_STRING:
	{
		{
			uint8_t m_count = 0;
			TGADGETMenu *gm = GADGETMenusList;
			while (gm != NULL)
			{
				m_count++;
				gm = gm->Next;
			}
			*(Am->Data.PointerString) = "m(" + String(m_count) + ") Id(";			
		}
		
		{
			uint8_t id_count = 0;
			TGADGETInputDialog *gid = GADGETInputDialogsList;
			while (gid != NULL)
			{
				id_count++;
				gid = gid->Next;
			}
			*(Am->Data.PointerString) = *(Am->Data.PointerString) + String(id_count) + ")  ";			
		}
		
		
		

		
		res = true;
		break;
	}
	default:
		break;
	}
	return res;
}



#pragma region INPUTDIALOG
TGADGETInputDialog *GUIGADGET_FindInputDialogByIDWindow(int8_t AIDWindow)
{
	TGADGETInputDialog *m = GADGETInputDialogsList;
	while (m != NULL)
	{
		if (m->IDWindow == AIDWindow)
		{
			return m;
		}
		m = m->Next;

	}
	return NULL;
}

TGADGETInputDialog *GUIGADGET_FindInputDialogByWindowClass(TWindowClass *Aw)
{
	TGADGETInputDialog *m = GADGETInputDialogsList;
	while (m != NULL)
	{
		if (m->WindowClass == Aw)
		{
			return m;
		}
		m = m->Next;

	}
	return NULL;
}

TGADGETInputDialog *GUIGADGET_FindInputDialogByOwnerTaskDef(TTaskDef *ATaskDef, int8_t AIDInputDialog)
{
	TGADGETInputDialog *m = GADGETInputDialogsList;
	while (m != NULL)
	{
		if ((m->OwnerTaskDef == ATaskDef) && (m->IDInputDialog==AIDInputDialog)) return m;
		m = m->Next;

	}
	return NULL;
}

TGADGETInputDialog *GUIGADGET_CreateInputDialog(TTaskDef *AOwnerTaskDef, int8_t AIDInputDialog, bool Amodal)
{
	TGADGETInputDialog *m = GUIGADGET_FindInputDialogByOwnerTaskDef(AOwnerTaskDef, AIDInputDialog);
	if (m == NULL)
	{
		m = new TGADGETInputDialog(AOwnerTaskDef, AIDInputDialog,Amodal);
	}
	return m;
}

void GUIGADGET_DestroyInputDialog(TGADGETInputDialog **Amenu)
{
	if (*Amenu != NULL)
	{
		//void* t = Amenu;
		
		delete(*Amenu);
		*Amenu = NULL;
		//board.SendMessage_FREEPTR(Amenu);
	}
}

TGADGETInputDialog::TGADGETInputDialog(TTaskDef *AOwnerTaskDef, int8_t AIDInputDialog,bool Amodal)
{
#ifdef XB_GUI
	ADD_TO_LIST(GADGETInputDialogsList, TGADGETInputDialog);

	
	OwnerTaskDef = AOwnerTaskDef;
	IDInputDialog = AIDInputDialog;

	TMessageBoard mb;	xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_INPUTDIALOG;
	mb.Data.InputDialogData.TypeInputDialogAction = ida_INIT_INPUTDIALOG;
	mb.Data.InputDialogData.IDInputDialog = AIDInputDialog;
	
	if (board.DoMessage(&mb, true, NULL, AOwnerTaskDef))
	{
		if (mb.Data.InputDialogData.ActionData.InputDialogInitData.TypeInputVar != tivNoDef)
		{
			TypeInputVar = mb.Data.InputDialogData.ActionData.InputDialogInitData.TypeInputVar;
			MaxLengthVar = mb.Data.InputDialogData.ActionData.InputDialogInitData.MaxLength;
			MinMax.uint32MinMax.Max = 0;
			MinMax.uint32MinMax.Min = 0;

			switch (TypeInputVar)
			{
			case tivDynArrayChar1:
			case tivDynArrayChar2:
			case tivDynArrayChar3:
				{
					InputVar.DynArrayChar = (char*)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
					EditVar = String(InputVar.DynArrayChar);
					break;
				}
			case tivString:
			{
				InputVar.DynString = (String*)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
				EditVar = *InputVar.DynString;

				break;
			}
			case tivIP:
				{
					InputVar.IP = (IPAddress *)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
					
#if defined(__riscv64) || defined(ARDUINO_ARCH_STM32)
					{
						char szRet[16];
						sprintf(szRet, "%u.%u.%u.%u", ((uint8_t *)InputVar.uInt32)[0], ((uint8_t *)InputVar.uInt32)[1], ((uint8_t *)InputVar.uInt32)[2], ((uint8_t *)InputVar.uInt32)[3]);
						EditVar = String(szRet);
					}
#else
					EditVar = InputVar.IP->toString();
#endif
					break;
				}
			case tivIP_U32:
			{
				InputVar.uInt32= (uint32_t *)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;

#if defined(__riscv64) || defined(ARDUINO_ARCH_STM32)
				{
					char szRet[16];
					sprintf(szRet, "%u.%u.%u.%u", ((uint8_t*)InputVar.uInt32)[0], ((uint8_t*)InputVar.uInt32)[1], ((uint8_t*)InputVar.uInt32)[2], ((uint8_t*)InputVar.uInt32)[3]);
					EditVar = String(szRet);
				}
#else
				EditVar = InputVar.IP->toString();
#endif
				break;
			}
			case tivUInt32:
				{
					InputVar.uInt32 = (uint32_t *)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
					EditVar = String((uint32_t)(*InputVar.uInt32));
					MinMax.uint32MinMax = mb.Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint32MinMax;
					break;
				}
			case tivUInt16:
				{
					InputVar.uInt16 = (uint16_t *)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
					EditVar = String((uint16_t)(*InputVar.uInt16));
					MinMax.uint16MinMax = mb.Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint16MinMax;
					break;
				}
			case tivInt16:
			{
				InputVar.Int16 = (int16_t*)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
				EditVar = String((int16_t)(*InputVar.Int16));
				MinMax.int16MinMax = mb.Data.InputDialogData.ActionData.InputDialogInitData.MinMax.int16MinMax;
				break;
			}
			case tivUInt8:
				{
					InputVar.uInt8 = (uint8_t *)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
					EditVar = String((uint8_t)(*InputVar.uInt8));
					MinMax.uint8MinMax = mb.Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint8MinMax;
					break;
				}
			case tivUInt8_HEX:
			{
				InputVar.uInt8 = (uint8_t*)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
				EditVar = String((uint8_t)(*InputVar.uInt8),HEX);
				MinMax.uint8MinMax = mb.Data.InputDialogData.ActionData.InputDialogInitData.MinMax.uint8MinMax;
				break;
			}
			case tiv_double:
			{
				InputVar._double = (double *)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
				EditVar = String(*InputVar._double,10);
				StringTrimRight(&EditVar, '0');
				break;
			}
			case tiv_udouble:
			{
				InputVar._udouble = (double*)mb.Data.InputDialogData.ActionData.InputDialogInitData.DataPointer;
				EditVar = String(*InputVar._udouble, 10);
				StringTrimRight(&EditVar, '0');
				break;
			}
			default: break;
			}
			CursorPosInputVar = EditVar.length();
		
			TWindowClass *Amodalwin = NULL;
			if (Amodal)
			{
				Amodalwin = GUI_FindWindowByActive();
			}

			IDWindow = GUI_GetUniqueIDWindow(&XB_GUIGADGET_DefTask);
			WindowClass = GUI_WindowCreate(&XB_GUIGADGET_DefTask, IDWindow, true, Amodalwin, false);
		}
		else
		{
			board.Log("In the initiation, no data type was given to input dialog.", true, true, tlError);	
		}

	}
	else
	{
		board.Log("Task not answer.",true,true,tlError);
	}
#endif
}

TGADGETInputDialog::~TGADGETInputDialog()
{
	DELETE_FROM_LIST(GADGETInputDialogsList);
}

void TGADGETInputDialog::EnterVAR()
{
#ifdef XB_GUI
	switch (TypeInputVar)
	{
	case tivString:
	{
		*InputVar.DynString = EditVar;
		break;
	}
	case tivDynArrayChar1:
	case tivDynArrayChar2:
	case tivDynArrayChar3:
	{
		EditVar.toCharArray(InputVar.DynArrayChar,MaxLengthVar,0);
		break;
	}
	case tivIP:
	{
		IPAddress ip;
		if (ip.fromString(EditVar))
		{
			*InputVar.IP = ip;
		}
		else
		{
			board.Log("Input IP is Error...", true, true, tlError);
		}
		break;
	}
	case tivIP_U32:
	{
		IPAddress ip;
		if (ip.fromString(EditVar))
		{
			*InputVar.uInt32 = ip;
		}
		else
		{
			board.Log("Input IP is Error...", true, true, tlError);
		}
		break;
	}
	case tivUInt32:
	{
		uint32_t v = 0;// (uint32_t)EditVar.toInt();
		StringToUINT(EditVar.c_str(), &v);
		if ((v >= MinMax.uint32MinMax.Min) && (v <= MinMax.uint32MinMax.Max))
		{
			*InputVar.uInt32 = v; 
		}
		else
		{
			board.Log("Input Value is out of range...", true, true, tlError);
		}
		break;
	}
	case tivUInt16:
	{
		uint32_t vv = 0;
		StringToUINT(EditVar.c_str(), &vv);
		uint16_t v = (uint16_t)vv;
		//uint16_t v = (uint16_t)EditVar.toInt(); 
		if ((v >= MinMax.uint16MinMax.Min) && (v <= MinMax.uint16MinMax.Max))
		{
			*InputVar.uInt16 = v; 
		}
		else
		{
			board.Log("Input Value is out of range...", true, true, tlError);
		}
		break;
	}
	case tivInt16:
	{
		long vv = 0;
		vv = EditVar.toInt();
		int16_t v = (int16_t)vv;
		if ((v >= MinMax.int16MinMax.Min) && (v <= MinMax.int16MinMax.Max))
		{
			*InputVar.Int16 = v;
		}
		else
		{
			board.Log("Input Value is out of range...", true, true, tlError);
		}
		break;
	}
	case tivUInt8:
		{
			uint32_t vv = 0;
			StringToUINT(EditVar.c_str(), &vv);
			uint8_t v = (uint8_t)vv;

			//uint32_t v = (uint32_t)EditVar.toInt(); 
			if ((v >= MinMax.uint8MinMax.Min) && (v <= MinMax.uint8MinMax.Max))
			{
				*InputVar.uInt8 = v; 
			}
			else
			{
				board.Log("Input Value is out of range...", true, true, tlError);
			}
			break;
		}
	case tivUInt8_HEX:
	{
		uint32_t vv = 0;
		hexstrTouint32((char*)EditVar.c_str(), 2, &vv);
		uint8_t v = (uint8_t)vv;
		
		if ((v >= MinMax.uint8MinMax.Min) && (v <= MinMax.uint8MinMax.Max))
		{
			*InputVar.uInt8 = v;
		}
		else
		{
			board.Log("Input Value is out of range...", true, true, tlError);
		}
		break;
	}
	case tiv_double:
	{
		double vv = strtodouble(EditVar);
		*InputVar._double=vv;
		break;
	}
	case tiv_udouble:
	{
		double vv = strtodouble(EditVar);
		if (vv>= 0.0f)
		{
			*InputVar._udouble = vv;
		}
		else
		{
			board.Log("Input Value is not unsigned...", true, true, tlError);
		}
		break;
	}

	default: break;
	}
	GUI_RepaintAllWindows();
#endif
}

void TGADGETInputDialog::SendEnterMessageToOwnerTask()
{
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_INPUTDIALOG;
	mb.fromTask = NULL;
	mb.Data.InputDialogData.IDInputDialog = IDInputDialog;
	mb.Data.InputDialogData.TypeInputDialogAction = ida_ENTER_DIALOG;
	board.DoMessage(&mb, true, NULL, OwnerTaskDef);
}

void TGADGETInputDialog::SendEscapeMessageToOwnerTask()
{
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_INPUTDIALOG;
	mb.fromTask = NULL;
	mb.Data.InputDialogData.IDInputDialog = IDInputDialog;
	mb.Data.InputDialogData.TypeInputDialogAction = ida_ESCAPE_DIALOG;
	board.DoMessage(&mb, true, NULL, OwnerTaskDef);
}

void TGADGETInputDialog::SendChangeValueMessageToOwnerTask()
{
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_INPUTDIALOG;
	mb.fromTask = NULL;
	mb.Data.InputDialogData.IDInputDialog = IDInputDialog;
	mb.Data.InputDialogData.TypeInputDialogAction = ida_CHANGE_VALUE;
	mb.Data.InputDialogData.ActionData.InputDialogChangeValue.Value = &EditVar;
	board.DoMessage(&mb, true, NULL, OwnerTaskDef);
}


void TGADGETInputDialog::PaintVar()
{
	if (WindowClass != NULL)
	{
		WindowClass->BeginDraw();

		uint8_t l = EditVar.length();
		uint8_t x = ((WindowClass->Width - 2) / 2) - ((MaxLengthVar + 2) / 2);

		
		if (TypeInputVar == tivUInt8_HEX)
		{
			WindowClass->GoToXY(x-1, 3);
			WindowClass->PutChar('>');
			WindowClass->PutChar('$');

		}
		else
		{
			WindowClass->GoToXY(x, 3);
			WindowClass->PutChar('>');
		}

		
		WindowClass->SetBoldChar();

		if ((TypeInputVar == tivIP) || (TypeInputVar == tivIP_U32))
		{
			IPAddress ip;
			if (ip.fromString(EditVar))
			{
				WindowClass->SetTextColor(tfcYellow);
			}
			else
			{
				WindowClass->SetTextColor(tfcRed);
			}
		}

		if (TypeInputVar == tivUInt32)
		{

			uint32_t v = 0;
			StringToUINT(EditVar.c_str(), &v);

			if ((v >= MinMax.uint32MinMax.Min) && (v <= MinMax.uint32MinMax.Max))
			{
				WindowClass->SetTextColor(tfcYellow);
			}
			else
			{
				WindowClass->SetTextColor(tfcRed);
			}
		}

		if (TypeInputVar == tivUInt16)
		{
			uint32_t vv = 0;
			StringToUINT(EditVar.c_str(), &vv);
			uint16_t v = (uint16_t)vv; 

			if ((v >= MinMax.uint16MinMax.Min) && (v <= MinMax.uint16MinMax.Max))
			{
				WindowClass->SetTextColor(tfcYellow);
			}
			else
			{
				WindowClass->SetTextColor(tfcRed);
			}
		}

		if (TypeInputVar == tivInt16)
		{
			long vv = 0;
			vv=EditVar.toInt();
			if ((vv > 32767) || (vv < -32768))
			{
				WindowClass->SetTextColor(tfcRed);
			}
			else
			{
				int16_t v = (int16_t)vv;
				if ((v >= MinMax.int16MinMax.Min) && (v <= MinMax.int16MinMax.Max))
				{
					WindowClass->SetTextColor(tfcYellow);
				}
				else
				{
					WindowClass->SetTextColor(tfcRed);
				}
			}
		}


		if (TypeInputVar == tivUInt8)
		{
			//uint32_t v = EditVar.toInt(); 
			uint32_t v = 0;
			StringToUINT(EditVar.c_str(), &v);


			if (v > 255)
			{
				WindowClass->SetTextColor(tfcRed);
			}
			else
			{
				uint8_t v8 = (uint8_t)v; 
				if ((v8 >= MinMax.uint8MinMax.Min) && (v8 <= MinMax.uint8MinMax.Max))
				{
					WindowClass->SetTextColor(tfcYellow);
				}
				else
				{
					WindowClass->SetTextColor(tfcRed);
				}
			}
		}

		if (TypeInputVar == tivUInt8_HEX)
		{
			uint32_t v = 0;
			hexstrTouint32((char *)EditVar.c_str(), 2, &v);

			//board.Log(String(EditVar+" | "+String(v)).c_str(), true, true);

			if (v > 255)
			{
				WindowClass->SetTextColor(tfcRed);
			}
			else
			{
				uint8_t v8 = (uint8_t)v;
				if ((v8 >= MinMax.uint8MinMax.Min) && (v8 <= MinMax.uint8MinMax.Max))
				{
					WindowClass->SetTextColor(tfcYellow);
				}
				else
				{
					WindowClass->SetTextColor(tfcRed);
				}
			}
		}


		for (uint8_t i = 0; i < MaxLengthVar; i++)
		{
			if (i < l)
			{
				WindowClass->PutChar(EditVar[i]);

			}
			else
			{
				WindowClass->PutChar(' ');
			}
		}
		WindowClass->SetTextColor(tfcYellow);
		WindowClass->SetNormalChar();
		WindowClass->PutChar('<');
		WindowClass->GoToXY(x + 1, 4);

		for (uint8_t i = 0; i <= MaxLengthVar; i++)
		{
			if (i == CursorPosInputVar)
			{
				WindowClass->PutChar('^');

			}
			else
			{
				WindowClass->PutChar(' ');
			}
		}

		WindowClass->EndDraw();
	}
}

bool TGADGETInputDialog::EditInsertChar(char Achar)
{
	{
		String str = EditVar.substring(0, CursorPosInputVar) + Achar + EditVar.substring(CursorPosInputVar, EditVar.length());
		EditVar = str.substring(0,MaxLengthVar);
	}
	MoveEditCursor(1);


	return true;
}

bool TGADGETInputDialog::MoveEditCursor(int8_t Astep)
{
	if (Astep < 0)
	{
		if (CursorPosInputVar == 0) return true;
		CursorPosInputVar += Astep;
		if (CursorPosInputVar < 0) CursorPosInputVar = 0;

	}
	else if (Astep > 0)
	{
		if (CursorPosInputVar == EditVar.length()) return true;
		CursorPosInputVar += Astep;
		if (CursorPosInputVar > EditVar.length()) CursorPosInputVar = EditVar.length();

	}
	return true;
}

bool TGADGETInputDialog::GetCaptionInputDialogString(String &Astr)
{
#ifdef XB_GUI
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_INPUTDIALOG;
	mb.Data.InputDialogData.TypeInputDialogAction = ida_GET_CAPTION_STRING;
	mb.Data.InputDialogData.IDInputDialog = IDInputDialog;
	mb.Data.InputDialogData.ActionData.InputDialogCaptionData.PointerString = &Astr;
	if (!board.DoMessage(&mb, true, NULL, OwnerTaskDef))
	{
		return false;
	}
#endif
	return true;
}

bool TGADGETInputDialog::GetDescriptionInputDialogString(String &Astr)
{
#ifdef XB_GUI
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);

	mb.IDMessage = IM_INPUTDIALOG;
	mb.Data.InputDialogData.TypeInputDialogAction = ida_GET_DESCRIPTION_STRING;
	mb.Data.InputDialogData.IDInputDialog = IDInputDialog;
	mb.Data.InputDialogData.ActionData.InputDialogDescriptionData.PointerString = &Astr;
	if (!board.DoMessage(&mb, true, NULL, OwnerTaskDef))
	{
		return false;
	}
#endif
	return true;
}


#pragma endregion

#pragma region MENU

bool GUIGADGET_IsMainMenu(TTaskDef* Ataskdef)
{
#ifdef XB_GUI
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.Data.MenuData.TypeMenuAction = tmaIS_MAINMENU;
	mb.Data.MenuData.IDMenu = 0;
	mb.Data.MenuData.ActionData.MenuIsMainData.is = false;
	bool res = board.DoMessage(&mb, true, NULL, Ataskdef);
	return mb.Data.MenuData.ActionData.MenuIsMainData.is;
#endif
}


bool GUIGADGET_OpenMainMenu(TTaskDef *Ataskdef,Tx Axpos,Ty Aypos)
{
#ifdef XB_GUI
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.Data.MenuData.TypeMenuAction = tmaOPEN_MAINMENU;
	mb.Data.MenuData.IDMenu = 0;
	mb.Data.MenuData.ActionData.MenuOpenMainData.X = Axpos;
	mb.Data.MenuData.ActionData.MenuOpenMainData.Y = Aypos;
	
	return board.DoMessage(&mb, true, NULL, Ataskdef);
#endif
}

bool GUIGADGET_CloseMainMenu(TTaskDef *Ataskdef)
{
#ifdef XB_GUI
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.Data.MenuData.TypeMenuAction = tmaCLOSE_MAINMENU;
	mb.Data.MenuData.IDMenu = 0;
	return board.DoMessage(&mb, true, NULL, Ataskdef);
#else
	return true;
#endif
}

void GUIGADGET_RepaintMenu(TGADGETMenu *Am)
{
	if (Am != NULL)
	{
		if (Am->WindowClass != NULL)
		{
			Am->WindowClass->RepaintCounter++;
			Am->WindowClass->RepaintDataCounter++;
		}
	}	
}

TGADGETMenu *GUIGADGET_FindMenuByIDWindow(int8_t AIDWindow)
{
	TGADGETMenu *m = GADGETMenusList;
	while (m != NULL)
	{
		if (m->IDWindow == AIDWindow)
		{
			return m;
		}
		m = m->Next;

	}
	return NULL;
}

TGADGETMenu *GUIGADGET_FindMenuByWindowClass(TWindowClass *Aw)
{
	TGADGETMenu *m = GADGETMenusList;
	while (m != NULL)
	{
		if (m->WindowClass == Aw)
		{
			return m;
		}
		m = m->Next;

	}
	return NULL;
}

TGADGETMenu *GUIGADGET_FindMenuByOwnerTaskDef(TTaskDef *ATaskDef,uint8_t AIDMenu)
{
	TGADGETMenu *m = GADGETMenusList;
	while (m != NULL)
	{
		if ((m->OwnerTaskDef == ATaskDef) && (m->IDMenu== AIDMenu)) return m;
		m = m->Next;

	}
	return NULL;
}

TGADGETMenu *GUIGADGET_CreateMenu(TTaskDef *AOwnerTaskDef,int8_t AIDMenu,bool Amodal,int16_t Ax, int16_t Ay)
{
	TGADGETMenu *m = GUIGADGET_FindMenuByOwnerTaskDef(AOwnerTaskDef,AIDMenu);

	if (m != NULL)
	{
		if (m->MenuIsInit)	
		{
			if (m->WindowClass != NULL)
			{
				m->WindowClass->SetActive();
				return m;
			}
		}
		GUIGADGET_DestroyMenu(&m);
	}
	
	if (m == NULL)
	{
		m = new TGADGETMenu(AOwnerTaskDef, AIDMenu, Amodal,Ax,Ay);
	}
	
	if (!m->MenuIsInit)	
	{
		GUIGADGET_DestroyMenu(&m);
		return NULL;
	}
	
	return m;
}

void GUIGADGET_DestroyMenu(TGADGETMenu **Amenu)
{
	if (Amenu != NULL)
	{
		if (*Amenu != NULL)
		{
			board.SendMessage_FREEPTR(Amenu);
			delete(*Amenu);
			*Amenu = NULL;
		}
	}
}

void GUIGADGET_CloseMenu(TGADGETMenu **Amenu)
{
	if (*Amenu != NULL)
	{
		GUI_WindowDestroy(&(*Amenu)->WindowClass);
		*Amenu = NULL;
	}
}

bool TGADGETMenu::DoMenuInit()
{
	bool res=false;
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.Data.MenuData.TypeMenuAction = tmaGET_INIT_MENU;
	mb.Data.MenuData.IDMenu = IDMenu;
	mb.Data.MenuData.ActionData.MenuInitData.EscapeClose = true;
	mb.Data.MenuData.ActionData.MenuInitData.X = X;
	mb.Data.MenuData.ActionData.MenuInitData.Y = Y;
	mb.Data.MenuData.ActionData.MenuInitData.CurrentSelect = CurrentItem;
	
	res = board.DoMessage(&mb, true, NULL, OwnerTaskDef);
	if (res)
	{

		if (mb.Data.MenuData.ActionData.MenuInitData.X == WINDOW_POS_X_DEF)
			mb.Data.MenuData.ActionData.MenuInitData.X = X;

		if (mb.Data.MenuData.ActionData.MenuInitData.Y == WINDOW_POS_Y_DEF)
			mb.Data.MenuData.ActionData.MenuInitData.Y = Y;

		X= mb.Data.MenuData.ActionData.MenuInitData.X;
		Y= mb.Data.MenuData.ActionData.MenuInitData.Y;


		if (mb.Data.MenuData.ActionData.MenuInitData.ItemCount == 255)
		{
			ItemCount = GetItemsMenuCount();	
			if (ItemCount == 255) 
			{
				board.Log("The number of menu items is not specified", true, true, tlWarn);
				return false;
			}
			if (ItemCount == 0)
			{
				board.Log("The number of menu items is 0 count", true, true, tlWarn);
				return false;				
			}

		}
		else
		{
			if (mb.Data.MenuData.ActionData.MenuInitData.ItemCount == 0)
			{
				board.Log("The number of menu items is 0 count", true, true, tlWarn);
				return false;				
			}
			else
			{
				ItemCount = mb.Data.MenuData.ActionData.MenuInitData.ItemCount;	
			}
		}
		
		if (ItemCount > 0)
		{
			WidthItems = mb.Data.MenuData.ActionData.MenuInitData.Width - 2;
			if (WindowClass == NULL)
			{
				CurrentItem = mb.Data.MenuData.ActionData.MenuInitData.CurrentSelect;
			}
			else
			{
				if (CurrentItem >= ItemCount) CurrentItem = ItemCount - 1;		
			}
			
			if (WindowClass == NULL)
			{
				if (Modal)
				{
					ModalWindowClass = GUI_FindWindowByActive();
				}
				IDWindow = GUI_GetUniqueIDWindow(&XB_GUIGADGET_DefTask);
				WindowClass = GUI_WindowCreate(&XB_GUIGADGET_DefTask, IDWindow, true, ModalWindowClass, mb.Data.MenuData.ActionData.MenuInitData.EscapeClose,X,Y);
			}
			else
			{
				WindowClass->SetWindowSize(WindowClass->Width, ItemCount + 2);
			}

			TypePaintMenuGadget = tpmgAll;
		}
		else
		{
			SendEscapeMessageToOwnerTask();
		}
	}
	return res;
}

TGADGETMenu::TGADGETMenu(TTaskDef *AOwnerTaskDef, int8_t AIDMenu,bool Amodal, int16_t Ax, int16_t Ay)
{
#ifdef XB_GUI
	ADD_TO_LIST(GADGETMenusList, TGADGETMenu);
	OwnerTaskDef = AOwnerTaskDef;
	IDMenu = AIDMenu;
	WindowClass = NULL;
	IDWindow = 0;
	Modal = Amodal;
	ModalWindowClass = NULL;
	CurrentItem = 0;
	X = Ax;
	Y = Ay;
	
	MenuIsInit = DoMenuInit();
	if (!MenuIsInit)
	{
		board.Log("Menu not init...", true, true, tlError);
		if (WindowClass != NULL)
		{
			GUI_WindowDestroy(&WindowClass);
		}
	}
#endif
}

TGADGETMenu::~TGADGETMenu()
{
	DELETE_FROM_LIST(GADGETMenusList);
}

bool TGADGETMenu::GetItemMenuString(uint8_t Aindx, String &Astr, TTextAlignment *Atextalign, bool *Anmenu)
{
#ifdef XB_GUI
	if (Aindx < ItemCount)
	{
		TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
		mb.IDMessage = IM_MENU;
		mb.Data.MenuData.TypeMenuAction = tmaGET_ITEM_MENU_STRING;
		mb.Data.MenuData.IDMenu = IDMenu;
		mb.Data.MenuData.ActionData.MenuItemData.ItemIndex = Aindx;
		mb.Data.MenuData.ActionData.MenuItemData.PointerString = &Astr;
		if (Atextalign == NULL)
		{
			mb.Data.MenuData.ActionData.MenuItemData.TextAlignment = taLeft;
		}
		else
		{
			mb.Data.MenuData.ActionData.MenuItemData.TextAlignment = *Atextalign;	
		}

		if (Anmenu == NULL)
		{
			mb.Data.MenuData.ActionData.MenuItemData.NMenu = false;
		}
		else
		{
			mb.Data.MenuData.ActionData.MenuItemData.NMenu = *Anmenu;
		}



		if (!board.DoMessage(&mb, true, NULL, OwnerTaskDef))
		{
			return false;
		}

		if (Atextalign != NULL)
		{
			*Atextalign = mb.Data.MenuData.ActionData.MenuItemData.TextAlignment;
		}

		if (Anmenu != NULL)
		{
			*Anmenu = mb.Data.MenuData.ActionData.MenuItemData.NMenu;
		}

		return true;
	}
	else
	{
		return false;
	}
#else
	return true;
#endif
}

uint8_t TGADGETMenu::GetItemsMenuCount()
{
#ifdef XB_GUI
		TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
		String Astr = "";
		mb.IDMessage = IM_MENU;
		mb.Data.MenuData.TypeMenuAction = tmaGET_ITEM_MENU_STRING;
		mb.Data.MenuData.IDMenu = IDMenu;
		mb.Data.MenuData.ActionData.MenuItemData.ItemIndex = 255;
		mb.Data.MenuData.ActionData.MenuItemData.PointerString = &Astr;
		if (!board.DoMessage(&mb, true, NULL, OwnerTaskDef))
		{
			return 0;
		}
		return mb.Data.MenuData.ActionData.MenuItemData.ItemIndex;
#else
	return 0;
#endif
}

bool TGADGETMenu::GetCaptionMenuString(String &Astr)
{
#ifdef XB_GUI
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.Data.MenuData.TypeMenuAction = tmaGET_CAPTION_MENU_STRING;
	mb.Data.MenuData.IDMenu = IDMenu;
	mb.Data.MenuData.ActionData.MenuCaptionData.PointerString = &Astr;
	if (!board.DoMessage(&mb, true, NULL, OwnerTaskDef))
	{
		return false;
	}
	return true;
#endif
}

void TGADGETMenu::ChangeCurrentItemMenu(uint8_t Anewcurrentitem)
{
	if (TypePaintMenuGadget == tpmgNone)
	{
		LastItem = CurrentItem;
		CurrentItem = Anewcurrentitem;
		if (TypePaintMenuGadget != tpmgAll)	
		{
			TypePaintMenuGadget = tpmgOnlyLastCurrent;
		}
		if (WindowClass != NULL)
		{
			WindowClass->RepaintDataCounter++;
		}
	}
}

void TGADGETMenu::ClickItemMenu(uint8_t Aclickitem)
{
#ifdef XB_GUI
	if (Aclickitem < ItemCount)
	{
		String s;
		GetItemMenuString(Aclickitem, s);
		if (s != "-")
		{
			TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
			mb.IDMessage = IM_MENU;
			mb.Data.MenuData.TypeMenuAction = tmaCLICK_ITEM_MENU;
			mb.Data.MenuData.IDMenu = IDMenu;
			mb.Data.MenuData.ActionData.MenuClickData.ItemIndex = Aclickitem;
			mb.Data.MenuData.ActionData.MenuClickData.Close = false;
			mb.Data.MenuData.ActionData.MenuClickData.Repaint = false;
			board.DoMessage(&mb, true, NULL, OwnerTaskDef);

			if (WindowClass != NULL)
			{
				TypePaintMenuGadget = tpmgOnlyCurrent;
				WindowClass->RepaintDataCounter++;
			
				if (mb.Data.MenuData.ActionData.MenuClickData.Close)
				{
					GUI_WindowDestroy(&WindowClass);
					GUI_ClearDesktop();
					GUI_RepaintAllWindows();
				}
				if (mb.Data.MenuData.ActionData.MenuClickData.Repaint)
				{
					GUIGADGET_RepaintMenu(this);
				}
			}
		}
	}
#endif
}

void TGADGETMenu::ClickLeftItemMenu(uint8_t Aclickitem)
{
#ifdef XB_GUI
	if (Aclickitem < ItemCount)
	{
		String s;
		GetItemMenuString(Aclickitem, s);
		if (s != "-")
		{
			TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
			mb.IDMessage = IM_MENU;
			mb.Data.MenuData.TypeMenuAction = tmaCLICKLEFT_ITEM_MENU;
			mb.Data.MenuData.IDMenu = IDMenu;
			mb.Data.MenuData.ActionData.MenuClickLeftRightData.ItemIndex = Aclickitem;
			board.DoMessage(&mb, true, NULL, OwnerTaskDef);

			if (WindowClass != NULL)
			{
				TypePaintMenuGadget = tpmgOnlyCurrent;
				WindowClass->RepaintDataCounter++;
			}
		}
	}
#endif
}

void TGADGETMenu::ClickRightItemMenu(uint8_t Aclickitem)
{
#ifdef XB_GUI
	if (Aclickitem < ItemCount)
	{
		String s;
		GetItemMenuString(Aclickitem, s);
		if (s != "-")
		{
			TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
			mb.IDMessage = IM_MENU;
			mb.Data.MenuData.TypeMenuAction = tmaCLICKRIGHT_ITEM_MENU;
			mb.Data.MenuData.IDMenu = IDMenu;
			mb.Data.MenuData.ActionData.MenuClickLeftRightData.ItemIndex = Aclickitem;
			board.DoMessage(&mb, true, NULL, OwnerTaskDef);

			if (WindowClass != NULL)
			{
				TypePaintMenuGadget = tpmgOnlyCurrent;
				WindowClass->RepaintDataCounter++;
			}
		}
	}
#endif
}

void TGADGETMenu::InsertItemMenu(uint8_t Aclickitem)
{
#ifdef XB_GUI
	if (Aclickitem < ItemCount)
	{
		String s;
		GetItemMenuString(Aclickitem, s);
		if (s != "-")
		{
			TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
			mb.IDMessage = IM_MENU;
			mb.Data.MenuData.TypeMenuAction = tmaCLICKRIGHT_ITEM_MENU;
			mb.Data.MenuData.IDMenu = IDMenu;
			mb.Data.MenuData.ActionData.MenuClickLeftRightData.ItemIndex = Aclickitem;
			board.DoMessage(&mb, true, NULL, OwnerTaskDef);

			if (WindowClass != NULL)
			{
				TypePaintMenuGadget = tpmgOnlyCurrent;
				WindowClass->RepaintDataCounter++;
			}
		}
	}
#endif
}


bool TGADGETMenu::DeleteItemMenu(uint8_t Adelitem)
{
#ifdef XB_GUI
	if (Adelitem < ItemCount)
	{
		String s;
		GetItemMenuString(Adelitem, s);
		if (s != "-")
		{
			TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
			mb.IDMessage = IM_MENU;
			mb.Data.MenuData.TypeMenuAction = tmaDEL_ITEM_MENU;
			mb.Data.MenuData.IDMenu = IDMenu;
			mb.Data.MenuData.ActionData.MenuDelData.ItemIndex = Adelitem;
			mb.Data.MenuData.ActionData.MenuDelData.ReInit = false;
			if (board.DoMessage(&mb, true, NULL, OwnerTaskDef))
			{
				if (mb.Data.MenuData.ActionData.MenuDelData.ReInit)
				{
					if (!DoMenuInit())
					{
						if (WindowClass != NULL)
						{
							GUI_WindowDestroy(&WindowClass);
						}
						return false;
					}
					else
					{
						return true;
					}
					
				}
			}
		}
		else return true;
	}
#endif
	return false;
}

bool TGADGETMenu::SendEscapeMessageToOwnerTask()
{
	TMessageBoard mb;xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.fromTask = NULL;
	mb.Data.MenuData.IDMenu = IDMenu;
	mb.Data.MenuData.TypeMenuAction = tmaESCAPE_MENU;
	return board.DoMessage(&mb, true, NULL,OwnerTaskDef);
}

bool TGADGETMenu::SendInsertMessageToOwnerTask()
{
	TMessageBoard mb; xb_memoryfill(&mb, sizeof(TMessageBoard), 0);
	mb.IDMessage = IM_MENU;
	mb.fromTask = NULL;
	mb.Data.MenuData.IDMenu = IDMenu;
	mb.Data.MenuData.TypeMenuAction = tmaINSERT_MENUITEM;
	return board.DoMessage(&mb, true, NULL, OwnerTaskDef);
}


#pragma endregion
#endif