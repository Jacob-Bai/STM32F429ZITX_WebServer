/*
 * ui_files.c
 *
 *  Created on: 20 Jan. 2022
 *      Author: Jacob Bai
 */

#include "uifiles.h"

const char bundle_html[] = {
};

const char favicon_ico[] = {
};

const UI_FILE ui_bundle_html = { HTML, bundle_html, sizeof (bundle_html) };
const UI_FILE ui_favicon_ico = { GIF, favicon_ico, sizeof (favicon_ico) };
