/*****************************************************************************************************
* TFT_manager
*
* Helperclass for class TFT1_8 to handle fields
*
* v0.1 2014/03/16 - Initial release / jschick
*
* This file is part of tft_st7735
*
* tft_st7735 is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* tft_st7735 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with tft_st7735. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <algorithm>
#include "tft_manager.h"

/*
 * refreshFunction:
 *	called by for_each
 *********************************************************************************
 */
void refreshFunction (TFT_field* p)
{
  p->refresh();
}

/*
 * TFT_manager:
 *	Constructor of the class
 *********************************************************************************
 */
TFT_manager::TFT_manager()
{

}


/*
 * add:
 *	add new field to the list
 *********************************************************************************
 */
void TFT_manager::add(TFT_field* field)
{
    _field_list.push_back(field);

}

/*
 * refresh:
 *	loop through the list and refresh all elements
 *********************************************************************************
 */
void TFT_manager::refresh()
{
  for_each (_field_list.begin(), _field_list.end(), refreshFunction);
}
