/*
 * webserver.h
 *
 *  Created on: 20 Jan. 2022
 *      Author: Jacob Bai
 */

#ifndef UIFILES_H_
#define UIFILES_H_

#include <string.h>

typedef enum  {
	HTML,
	CSS,
	JS,
	GIF
} UI_File_Type;
typedef struct UI_File {
	UI_File_Type fileType;
	const char *ptr;
	size_t length;
} UI_FILE;

extern const UI_FILE ui_bundle_html;

// ui/
extern const UI_FILE ui_favicon_ico;
//extern const UI_FILE ui_styles_css;
//extern const UI_FILE ui_index_html;
//
//// ui/lib/js/
//extern const UI_FILE ui_lib_js_bootstrap_bundle_min_js;
//extern const UI_FILE ui_lib_js_masonry_pkgd_min_js;
//
//// ui/lib/css/
//extern const UI_FILE ui_lib_css_bootstrap_min_css;
//
//// ui/js/
//extern const UI_FILE ui_js_config_js;
//extern const UI_FILE ui_js_program_js;
//extern const UI_FILE ui_js_system_js;
//extern const UI_FILE ui_js_utils_js;

#endif
