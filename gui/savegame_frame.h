/*
 * Copyright (c) 1997 - 2001 Hansjoerg Malthaner
 *
 * This file is part of the Simutrans project under the artistic licence.
 * (see licence.txt)
 */

#ifndef gui_savegame_frame_h
#define gui_savegame_frame_h

#include <string.h>
#include <sys/stat.h>

#include "../tpl/slist_tpl.h"
#include "../tpl/vector_tpl.h"
#include "components/action_listener.h"
#include "components/gui_table.h"
#include "components/gui_scrollpane.h"
#include "components/gui_textinput.h"
#include "components/gui_divider.h"
#include "components/gui_label.h"
#include "components/gui_button.h"
#include "gui_frame.h"
#include "gui_container.h"

#include <string>

using std::string;


class gui_file_table_column_t : public gui_table_column_t
{
protected:
	bool pressed;
	virtual const char *get_text(const gui_table_row_t &row) const { (void) row; return ""; }
public:
	gui_file_table_column_t(coordinate_t size_) : gui_table_column_t(size_) { pressed = false; }
	virtual int compare_rows(const gui_table_row_t &row1, const gui_table_row_t &row2) const { return strcmp(get_text(row1), get_text(row2)); }
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row) = 0;
	bool get_pressed() const { return pressed; }
	void set_pressed(bool value) { pressed = value; }
};


class gui_file_table_button_column_t : public gui_file_table_column_t
{
protected:
	button_t btn;
public:
	gui_file_table_button_column_t(coordinate_t size_) : gui_file_table_column_t(size_) {}
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
	void set_text(const char *text) { btn.set_text(text); }
	void set_tooltip(const char *tooltip) { btn.set_tooltip(tooltip); }
};


class gui_file_table_label_column_t : public gui_file_table_column_t
{
protected:
	gui_label_t lbl;
public:
	gui_file_table_label_column_t(coordinate_t size_) : gui_file_table_column_t(size_) {}
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};


class gui_file_table_delete_column_t : public gui_file_table_button_column_t
{
public:
	gui_file_table_delete_column_t() : gui_file_table_button_column_t(14) {
		btn.set_text("X");
		btn.set_tooltip("Delete this file.");
	}
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};


class gui_file_table_action_column_t : public gui_file_table_button_column_t
{
protected:
	virtual const char *get_text(const gui_table_row_t &row) const;
public:
	gui_file_table_action_column_t() : gui_file_table_button_column_t(300) {
		btn.set_no_translate(true);
	}
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};


class gui_file_table_time_column_t : public gui_file_table_label_column_t
{
protected:
	virtual time_t get_time(const gui_table_row_t &row) const;
public:
	gui_file_table_time_column_t() : gui_file_table_label_column_t(120) {
		set_sort_descendingly(true);
	}
	virtual int compare_rows(const gui_table_row_t &row1, const gui_table_row_t &row2) const { return sgn(get_time(row1) - get_time(row2)); }
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};


class gui_file_table_row_t : public gui_table_row_t
{
	friend class gui_file_table_button_column_t;
	friend class gui_file_table_delete_column_t;
	friend class gui_file_table_action_column_t;
	friend class gui_file_table_time_column_t;
protected:
	string text;
	string name;
	string error;
	bool pressed;
	bool delete_enabled;
	struct stat info;
public:
	//gui_file_table_row_t();
	gui_file_table_row_t(const char *pathname, const char *buttontext, bool delete_enabled = true);
	const char *get_name() const { return name.c_str(); }
	void set_pressed(bool value) { pressed = value; }
	bool get_pressed() { return pressed; }
	void set_delete_enabled(bool value) { delete_enabled = value; }
	bool get_delete_enabled() { return delete_enabled; }
};


class gui_file_table_t : public	gui_table_t
{
protected:
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y);	
};

/**
 * Base class from wich all GUI dialogs to load/save generics can inherit from
 * @author Hansjoerg Malthaner
 * @author Makohs
 * @note Based on previous original work from the simutrans team and Hansjoerg Malthaner
 * @note When I refer to a "qualified" path I mean it can contain sub-directories or even fully qualified path. i.e. : "save/a.sve" or "c:\simutrans\scenario\file.nut"
 */
class savegame_frame_t : public gui_frame_t, action_listener_t
{
private:

	/**
	 * To avoid double mouse action
	 */
	bool in_action;

	/**
	 * Paths in wich this dialog will search for
	 */
	vector_tpl<std::string> paths;

	/**
	 * Input buffer for the text input component
	 */
	char ibuf[1024];

	/**
	 * Is default path defined?
	 */
	bool searchpath_defined;

	/**
	 * Default search path
	 */
	char searchpath[1024];

	/**
	 * Adds a section entry to the list
	 */
	void add_section(std::string &name);

	/**
	 * Extension of the files this dialog will use, can be NULL
	 * Can include or not the "." at start, will work on both cases
	 */
	const char *suffix;

	/**
	 * Search for directories (used in pak_selector)
	 */
	bool only_directories;

	/**
	 * Enable delete buttons
	 */
	bool delete_enabled;

	bool file_table_button_pressed;
	coordinates_t pressed_file_table_button;

	void press_file_table_button(coordinates_t &cell);
	void release_file_table_button();	
protected:

	gui_textinput_t input;
	gui_divider_t divider1;                               // 30-Oct-2001  Markus Weber    Added
	button_t savebutton;                                  // 29-Oct-2001  Markus Weber    Added
	button_t cancelbutton;                                // 29-Oct-2001  Markus Weber    Added
	gui_label_t fnlabel;        //filename                // 31-Oct-2001  Markus Weber    Added
	gui_file_table_t file_table;
	gui_container_t button_frame;
	gui_scrollpane_t scrolly;
	// use file_table instead of button_frame:
	bool use_table;

	/**
	 * Entries in list can be actual file entries or headers, that have a diferent look
	 */
	enum dirlist_item_t {LI_HEADER,LI_ENTRY};

	struct dir_entry_t
	{
		dir_entry_t(button_t* button_, button_t* del_, gui_label_t* label_, dirlist_item_t type_ = LI_ENTRY, const char *info_=NULL) :
			button(button_),
			del(del_),
			label(label_),
			type(type_),
			info(info_)
		{}

		button_t*      button;
		button_t*      del;
		gui_label_t*   label;
		dirlist_item_t type;

		/**
		* Contains a qualified path (might be relative) to the file, not just the name
		*/
		const char *info;
	};

	/**
	 * Returns extra file info
	 * @note filename is a qualified path
	 */
	virtual const char *get_info(const char *fname) = 0;

	/**
	 * Called on each entry, to be re-implemented on each sub-class
	 * @return if true, the file will be added to the list
	 * @note filename is a qualified path
	 */
	virtual bool check_file(const char *filename, const char *suffix);

	/**
	 * Called on each entry that passed the check
	 */
	void add_file(const char *path, const char *filename, const char *pak, const bool no_cutting_suffix);
	virtual void add_file(const char *fullpath, const char *filename, const bool not_cutting_suffix);

	/**
	 * Adds a directory to search in
	 */
	void add_path(const char *path);

	/**
	 * Called when the directory processing ends
	 */
	void list_filled();

	/**
	 * Internal list representing the file listing
	 */
	slist_tpl<dir_entry_t> entries;

	/**
	 * Internal counter representing the number of sections added to the list
	 */
	uint32 num_sections;

	/**
	 * Callback function that will be executed when the user clicks one of the entries in list
	 * @author Hansj�rg Malthaner
	 */
	virtual void action(const char *fullpath) = 0;

	/**
	 * Will be executed when the user presses the delete 'X' button.
	 * @return If true, then the dialogue will be closed
	 * @note on the pakselector, this action is re-used to load addons
	 * @author Volker Meyer
	 */
	virtual bool del_action(const char *fullpath);

	/**
	 * Sets the filename in the edit field
	 */
	void set_filename(const char *fn);

	/**
	 * Translates '/' into '\', in Windows systems, will capitalize the drive letter too.
	 */
	void cleanup_path(char *path);

	/**
	 * Outputs a shortened path removing characters in the middle of the input path, replacing them with "..."
	 * @param dest output will be written here
	 * @param orig input string
	 * @param max_size the string will be truncated to this length
	 */
	void shorten_path(char *dest, const char *orig, const size_t max_size);

	/**
	 * extracts file name from a full path
	 */
	std::string get_filename(const char *fullpath, const bool with_extension = true);

	virtual void init(const char *suffix, const char *path);

	/**
	 * called by fill_list():
	 */
	virtual void set_file_table_default_sort_order();

public:

	/**
	 * extracts base name from a full path
	 */
	std::string get_basename(const char *fullpath);

	/**
	 * Sets the gui components widths and coordinates
	 * @author Mathew Hounsell
	 * \date   11-Mar-2003
	 */
	virtual void set_fenstergroesse(koord groesse);

	/**
	 * @param suffix Filename suffix, i.e. ".sve", you can omit the intial dot, i.e. "sve", NULL to not enforce any extension.
	 * @param only_directories will just process directory entries, not files.
	 * @param path Default path to search at. If NULL, next call to add_path will define the default path.
	 * @param delete_enabled Determins if we'll add delete buttons to the frame.
	 * @author Hj. Malthaner
	 */
	savegame_frame_t(const char *suffix = NULL, bool only_directories = false, const char *path = NULL, const bool delete_enabled = true, bool use_table = false );

	virtual ~savegame_frame_t();

	/**
	 * Inherited from action_listener_t
	 */
	bool action_triggered(gui_action_creator_t*, value_t) OVERRIDE;

	/**
	 * Must catch open message to update list, since I am using virtual functions
	 */
	bool infowin_event(event_t const*) OVERRIDE;

	/**
	 * Start the directory processing
	 */
	void fill_list();
};

#endif
