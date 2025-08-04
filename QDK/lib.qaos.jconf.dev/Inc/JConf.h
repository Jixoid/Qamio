/*
  This file is part of JConf
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with JConf. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#pragma once

#include "Basis.h"


#ifdef __cplusplus
extern "C" {
#endif


// Types
typedef  point  jc_obj;

typedef  point  jc_val;
typedef  point  jc_stc;
typedef  point  jc_arr;


// Is
bool jc_IsVal(jc_obj obj);
bool jc_IsStc(jc_obj obj);
bool jc_IsArr(jc_obj obj);


// New / Dis
jc_val  jc_NewVal();
void    jc_DisVal(jc_val obj);

jc_stc  jc_NewStc();
void    jc_DisStc(jc_stc obj);

jc_arr  jc_NewArr();
void    jc_DisArr(jc_arr obj);


// Set/Get
char*   jc_ValGet(jc_val obj);
bool    jc_ValSet(jc_val obj, const char* value);
void    jc_DisStr(char *str);


jc_obj  jc_StcGet(jc_stc obj, const char* scope);
bool    jc_StcSet(jc_stc obj, const char* scope, jc_obj object);
bool    jc_StcDel(jc_stc obj, const char* scope);
u32     jc_StcC  (jc_stc obj);
jc_obj  jc_StcInd(jc_stc obj, u32 index, char** Name);


u32     jc_ArrC   (jc_arr obj);
jc_obj  jc_ArrGet (jc_arr obj, u32 index);
bool    jc_ArrSet (jc_arr obj, u32 index, jc_obj object);
bool    jc_ArrPush(jc_arr obj, jc_obj object);
bool    jc_ArrDel (jc_arr obj, u32 index);



// Parse
jc_stc  jc_Parse(const char* fpath);
jc_stc  jc_ParseRaw(const char* fpath);
jc_stc  jc_ParseBin(const char* fpath);

bool    jc_WriteRaw(const char* fpath, jc_stc data);
bool    jc_WriteBin(const char* fpath, jc_stc data);


#ifdef __cplusplus
}
#endif
