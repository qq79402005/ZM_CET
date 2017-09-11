/*************************************************************************/
/*  gd_script.h                                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#ifndef GD_SCRIPT_H
#define GD_SCRIPT_H

#include "gd_function.h"
#include "io/resource_loader.h"
#include "io/resource_saver.h"
#include "script_language.h"
class GDNativeClass : public Reference {

	OBJ_TYPE(GDNativeClass, Reference);

	StringName name;

protected:
	bool _get(const StringName &p_name, Variant &r_ret) const;
	static void _bind_methods();

public:
	_FORCE_INLINE_ const StringName &get_name() const { return name; }
	Variant _new();
	Object *instance();
	GDNativeClass(const StringName &p_name);
};

class GDScript : public Script {

	OBJ_TYPE(GDScript, Script);
	bool tool;
	bool valid;

	struct MemberInfo {
		int index;
		StringName setter;
		StringName getter;
	};

	friend class GDInstance;
	friend class GDFunction;
	friend class GDCompiler;
	friend class GDFunctions;
	friend class GDScriptLanguage;

	Variant _static_ref; //used for static call
	Ref<GDNativeClass> native;
	Ref<GDScript> base;
	GDScript *_base; //fast pointer access
	GDScript *_owner; //for subclasses

	Set<StringName> members; //members are just indices to the instanced script.
	Map<StringName, Variant> constants;
	Map<StringName, GDFunction *> member_functions;
	Map<StringName, MemberInfo> member_indices; //members are just indices to the instanced script.
	Map<StringName, Ref<GDScript> > subclasses;
	Map<StringName, Vector<StringName> > _signals;

#ifdef TOOLS_ENABLED

	Map<StringName, Variant> member_default_values;

	List<PropertyInfo> members_cache;
	Map<StringName, Variant> member_default_values_cache;
	Ref<GDScript> base_cache;
	Set<ObjectID> inheriters_cache;
	bool source_changed_cache;
	void _update_exports_values(Map<StringName, Variant> &values, List<PropertyInfo> &propnames);

#endif
	Map<StringName, PropertyInfo> member_info;

	GDFunction *initializer; //direct pointer to _init , faster to locate

	int subclass_count;
	Set<Object *> instances;
	//exported members
	String source;
	String path;
	String name;
	SelfList<GDScript> script_list;

	GDInstance *_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_isref, Variant::CallError &r_error);

	void _set_subclass_path(Ref<GDScript> &p_sc, const String &p_path);

#ifdef TOOLS_ENABLED
	Set<PlaceHolderScriptInstance *> placeholders;
	//void _update_placeholder(PlaceHolderScriptInstance *p_placeholder);
	virtual void _placeholder_erased(PlaceHolderScriptInstance *p_placeholder);
#endif

#ifdef DEBUG_ENABLED

	Map<ObjectID, List<Pair<StringName, Variant> > > pending_reload_state;

#endif

	bool _update_exports();

protected:
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	void _get_property_list(List<PropertyInfo> *p_properties) const;

	Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	//	void call_multilevel(const StringName& p_method,const Variant** p_args,int p_argcount);

	static void _bind_methods();

public:
	bool is_valid() const { return valid; }

	const Map<StringName, Ref<GDScript> > &get_subclasses() const { return subclasses; }
	virtual const Map<StringName, Variant> &get_constants() const { return constants; }
	virtual const Set<StringName> &get_members() const { return members; }
	const Map<StringName, GDFunction *> &get_member_functions() const { return member_functions; }
	const Ref<GDNativeClass> &get_native() const { return native; }

	virtual bool has_script_signal(const StringName &p_signal) const;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const;

	bool is_tool() const { return tool; }
	Ref<GDScript> get_base() const;

	const Map<StringName, MemberInfo> &debug_get_member_indices() const { return member_indices; }
	const Map<StringName, GDFunction *> &debug_get_member_functions() const; //this is debug only
	StringName debug_get_member_by_index(int p_idx) const;

	Variant _new(const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	virtual bool can_instance() const;

	virtual StringName get_instance_base_type() const; // this may not work in all scripts, will return empty if so
	virtual ScriptInstance *instance_create(Object *p_this);
	virtual bool instance_has(const Object *p_this) const;

	virtual bool has_source_code() const;
	virtual String get_source_code() const;
	virtual void set_source_code(const String &p_code);
	virtual void update_exports();

	virtual Error reload(bool p_keep_state = false);

	virtual String get_node_type() const;
	void set_script_path(const String &p_path) { path = p_path; } //because subclasses need a path too...
	Error load_source_code(const String &p_path);
	Error load_byte_code(const String &p_path);

	Vector<uint8_t> get_as_byte_code() const;

	bool get_property_default_value(const StringName &p_property, Variant &r_value) const;

	virtual ScriptLanguage *get_language() const;

	GDScript();
	~GDScript();
};

class GDInstance : public ScriptInstance {
	friend class GDScript;
	friend class GDFunction;
	friend class GDFunctions;
	friend class GDCompiler;

	Object *owner;
	Ref<GDScript> script;
#ifdef DEBUG_ENABLED
	Map<StringName, int> member_indices_cache; //used only for hot script reloading
#endif
	Vector<Variant> members;
	bool base_ref;

	void _ml_call_reversed(GDScript *sptr, const StringName &p_method, const Variant **p_args, int p_argcount);

public:
	virtual Object *get_owner() { return owner; }

	virtual bool set(const StringName &p_name, const Variant &p_value);
	virtual bool get(const StringName &p_name, Variant &r_ret) const;
	virtual void get_property_list(List<PropertyInfo> *p_properties) const;
	virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = NULL) const;

	virtual void get_method_list(List<MethodInfo> *p_list) const;
	virtual bool has_method(const StringName &p_method) const;
	virtual Variant call(const StringName &p_method, const Variant **p_args, int p_argcount, Variant::CallError &r_error);
	virtual void call_multilevel(const StringName &p_method, const Variant **p_args, int p_argcount);
	virtual void call_multilevel_reversed(const StringName &p_method, const Variant **p_args, int p_argcount);

	Variant debug_get_member_by_index(int p_idx) const { return members[p_idx]; }

	virtual void notification(int p_notification);

	virtual Ref<Script> get_script() const;

	virtual ScriptLanguage *get_language();

	void set_path(const String &p_path);

	void reload_members();

	GDInstance();
	~GDInstance();
};

class GDScriptLanguage : public ScriptLanguage {

	static GDScriptLanguage *singleton;

	Variant *_global_array;
	Vector<Variant> global_array;
	Map<StringName, int> globals;

	struct CallLevel {

		Variant *stack;
		GDFunction *function;
		GDInstance *instance;
		int *ip;
		int *line;
	};

	int _debug_parse_err_line;
	String _debug_parse_err_file;
	String _debug_error;
	int _debug_call_stack_pos;
	int _debug_max_call_stack;
	CallLevel *_call_stack;

	void _add_global(const StringName &p_name, const Variant &p_value);

	Mutex *lock;

	friend class GDScript;

	SelfList<GDScript>::List script_list;
	friend class GDFunction;

	SelfList<GDFunction>::List function_list;
	bool profiling;
	uint64_t script_frame_time;

public:
	int calls;

	bool debug_break(const String &p_error, bool p_allow_continue = true);
	bool debug_break_parse(const String &p_file, int p_line, const String &p_error);

	_FORCE_INLINE_ void enter_function(GDInstance *p_instance, GDFunction *p_function, Variant *p_stack, int *p_ip, int *p_line) {

		if (Thread::get_main_ID() != Thread::get_caller_ID())
			return; //no support for other threads than main for now

		if (ScriptDebugger::get_singleton()->get_lines_left() > 0 && ScriptDebugger::get_singleton()->get_depth() >= 0)
			ScriptDebugger::get_singleton()->set_depth(ScriptDebugger::get_singleton()->get_depth() + 1);

		if (_debug_call_stack_pos >= _debug_max_call_stack) {
			//stack overflow
			_debug_error = "Stack Overflow (Stack Size: " + itos(_debug_max_call_stack) + ")";
			ScriptDebugger::get_singleton()->debug(this);
			return;
		}

		_call_stack[_debug_call_stack_pos].stack = p_stack;
		_call_stack[_debug_call_stack_pos].instance = p_instance;
		_call_stack[_debug_call_stack_pos].function = p_function;
		_call_stack[_debug_call_stack_pos].ip = p_ip;
		_call_stack[_debug_call_stack_pos].line = p_line;
		_debug_call_stack_pos++;
	}

	_FORCE_INLINE_ void exit_function() {

		if (Thread::get_main_ID() != Thread::get_caller_ID())
			return; //no support for other threads than main for now

		if (ScriptDebugger::get_singleton()->get_lines_left() > 0 && ScriptDebugger::get_singleton()->get_depth() >= 0)
			ScriptDebugger::get_singleton()->set_depth(ScriptDebugger::get_singleton()->get_depth() - 1);

		if (_debug_call_stack_pos == 0) {

			_debug_error = "Stack Underflow (Engine Bug)";
			ScriptDebugger::get_singleton()->debug(this);
			return;
		}

		_debug_call_stack_pos--;
	}

	virtual Vector<StackInfo> debug_get_current_stack_info() {
		if (Thread::get_main_ID() != Thread::get_caller_ID())
			return Vector<StackInfo>();

		Vector<StackInfo> csi;
		csi.resize(_debug_call_stack_pos);
		for (int i = 0; i < _debug_call_stack_pos; i++) {
			csi[_debug_call_stack_pos - i - 1].line = _call_stack[i].line ? *_call_stack[i].line : 0;
			csi[_debug_call_stack_pos - i - 1].script = Ref<GDScript>(_call_stack[i].function->get_script());
		}
		return csi;
	}

	struct {

		StringName _init;
		StringName _notification;
		StringName _set;
		StringName _get;
		StringName _get_property_list;
		StringName _script_source;

	} strings;

	_FORCE_INLINE_ int get_global_array_size() const { return global_array.size(); }
	_FORCE_INLINE_ Variant *get_global_array() { return _global_array; }
	_FORCE_INLINE_ const Map<StringName, int> &get_global_map() { return globals; }

	_FORCE_INLINE_ static GDScriptLanguage *get_singleton() { return singleton; }

	virtual String get_name() const;

	/* LANGUAGE FUNCTIONS */
	virtual void init();
	virtual String get_type() const;
	virtual String get_extension() const;
	virtual Error execute_file(const String &p_path);
	virtual void finish();

	/* EDITOR FUNCTIONS */
	virtual void get_reserved_words(List<String> *p_words) const;
	virtual void get_comment_delimiters(List<String> *p_delimiters) const;
	virtual void get_string_delimiters(List<String> *p_delimiters) const;
	virtual String get_template(const String &p_class_name, const String &p_base_class_name) const;
	virtual bool validate(const String &p_script, int &r_line_error, int &r_col_error, String &r_test_error, const String &p_path = "", List<String> *r_functions = NULL) const;
	virtual Script *create_script() const;
	virtual bool has_named_classes() const;
	virtual int find_function(const String &p_function, const String &p_code) const;
	virtual String make_function(const String &p_class, const String &p_name, const StringArray &p_args) const;
	virtual Error complete_code(const String &p_code, const String &p_base_path, Object *p_owner, List<String> *r_options, String &r_call_hint);
	virtual void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const;
	virtual void add_global_constant(const StringName &p_variable, const Variant &p_value);

	/* DEBUGGER FUNCTIONS */

	virtual String debug_get_error() const;
	virtual int debug_get_stack_level_count() const;
	virtual int debug_get_stack_level_line(int p_level) const;
	virtual String debug_get_stack_level_function(int p_level) const;
	virtual String debug_get_stack_level_source(int p_level) const;
	virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
	virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
	virtual void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1);
	virtual ScriptInstance *debug_get_stack_level_instance(int p_level);
	virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1);

	virtual void reload_all_scripts();
	virtual void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload);

	virtual void frame();

	virtual void get_public_functions(List<MethodInfo> *p_functions) const;
	virtual void get_public_constants(List<Pair<String, Variant> > *p_constants) const;

	virtual void profiling_start();
	virtual void profiling_stop();

	virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max);
	virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max);

	/* LOADER FUNCTIONS */

	virtual void get_recognized_extensions(List<String> *p_extensions) const;

	GDScriptLanguage();
	~GDScriptLanguage();
};

class ResourceFormatLoaderGDScript : public ResourceFormatLoader {
public:
	virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = NULL);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class ResourceFormatSaverGDScript : public ResourceFormatSaver {
public:
	virtual Error save(const String &p_path, const RES &p_resource, uint32_t p_flags = 0);
	virtual void get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const;
	virtual bool recognize(const RES &p_resource) const;
};

#endif // GD_SCRIPT_H