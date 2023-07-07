#include <NanoGL.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
int chdir(const char *path)
{
	return (SetCurrentDirectory(path) == TRUE) ? 0 : -1;
}

#define PATH_SEPARATOR '\\'
#else
#include <unistd.h>
#define PATH_SEPARATOR '/'
char *_fullpath(char *buffer, const char *path, size_t maxLength)
{
	char *abspath = realpath(path, NULL);
	if (buffer == NULL)
	{
		return abspath;
	}
	else {
		size_t len = min(strlen(abspath)+1, maxLength);
		if (len > 0) {
			strncpy(buffer, abspath, len - 1);
			buffer[len - 1] = '\0';
			free(abspath);
			return buffer;
		} else
		{
			free(abspath);
			return NULL;
		}
	}
}
#endif

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/string.h>

extern mrb_value mrb_get_backtrace(mrb_state *mrb, mrb_value self);

extern void mrb_nanogl_gem_init(mrb_state* mrb);
extern void mrb_nanogl_gem_final(mrb_state* mrb);

static mrb_value mrb_printstr(mrb_state *mrb, mrb_value self) {
	(void)self;
	mrb_value argv;
	mrb_get_args(mrb, "o", &argv);
	if (mrb_string_p(argv)) {
		struct RString *str = mrb_str_ptr(argv);
		Debug.PrintError("%s", RSTR_PTR(str));
	}
	return argv;
}

static FILE *fp = NULL;
static int mrb_ai = 0;
static mrb_state *mrb = NULL;
static mrbc_context *c = NULL;

NanoGL_Initialize() {
	mrb = mrb_open();
	if (mrb == NULL) {
		return;
	}
	mrb_ai = mrb_gc_arena_save(mrb);

	// Hook __printstr__ in Kernel 
	struct RClass *krn = mrb->kernel_module;
	mrb_undef_method(mrb, krn, "__printstr__");
	mrb_define_method(mrb, krn, "__printstr__", (mrb_func_t)mrb_printstr, MRB_ARGS_REQ(1));

	mrb_nanogl_gem_init(mrb);
	const char *HELP = "\n \
module NanoGL\n \
	module Video\n \
		def self.Draw(&block)\n \
			if block_given?\n \
				loop do\n \
					self.Drawing()\n \
					yield if block_given?\n \
				end\n \
			else\n \
				self.Drawing()\n \
			end\n \
		end\n \
		def self.Path(&block)\n \
			self.BeginPath()\n \
			yield if block_given?\n \
			self.ClosePath()\n \
		end\n \
	end\n \
end\n \
\n \
class << self\n \
	alias :original_require :require\n \
	def require(s)\n \
		if ['NanoGL', 'tempfile', 'io', 'dir'].any? { |name| s.casecmp(name) == 0 } \n \
			return false\n \
		else\n \
			original_require(s)\n \
		end\n \
	end\n \
	def require_relative(s)\n \
		begin\n \
			cur = Dir.pwd\n \
	        real = File.expand_path(s, cur)\n \
			newdir = File.dirname(real)\n \
	        s = './' + real[newdir.length .. -1]\n \
			Dir.chdir(newdir)\n \
			original_require(s)\n \
		ensure\n \
			Dir.chdir(cur)\n \
		end\n \
	end\n \
	def sleep(sec)\n \
		Video.Sleep(sec)\n \
	end\n \
	def exit\n \
		System.Quit\n \
	end\n \
end\n \
";
	const char* utf8 = mrb_utf8_from_locale(HELP, strlen(HELP));
	if (utf8 == NULL)
	{
		return;
	}
	mrb_load_string(mrb, utf8);
	mrb_utf8_free((void*)utf8);
	int cnt = System.GetCommandLineArgCount();

	char *path = NULL;
	if (cnt >= 2) {
		path = strdup(System.GetCommandLineArgValue(1));
	} else if (cnt == 1) {
		string_t tp = String.Create(System.GetCommandLineArgValue(0));
		char *p = strrchr(tp.c_str, '.');
		if (p != NULL) {
			*p = '\0';
			String.JoinDirect(&tp, ".rb");
			path = strdup(tp.c_str);
			String.Free(tp);
		}
	}
	if (path != NULL) {
		fp = fopen(path, "rb");
		if (fp == NULL) {
			Debug.PrintError("Cannot open file : %s\n", path);
		}
		else {
			char *dir = _fullpath(NULL, path, 0);
			char *p = strrchr(dir, PATH_SEPARATOR);
			*p = '\0';
			chdir(dir);
			free(dir);
			c = mrbc_context_new(mrb);
			mrbc_filename(mrb, c, path);
			c->capture_errors = TRUE;
		}
		free(path);
	}
}

static void check_exc(void)
{
	if (mrb->exc) {
		mrb_value exc = mrb_obj_value(mrb->exc);
		mrb_value backtrace = mrb_get_backtrace(mrb, exc);
		Debug.PrintError("%s", mrb_str_to_cstr(mrb, mrb_inspect(mrb, backtrace)));
		mrb_value inspect = mrb_inspect(mrb, exc);
		Debug.PrintError("%s", mrb_str_to_cstr(mrb, inspect));
		mrb->exc = NULL;
	}

}

NanoGL_Finalize() {
	if (mrb != NULL) {
		check_exc();

		if (c != NULL) {
			mrbc_context_free(mrb, c);
		}
		if (fp != NULL) {
			fclose(fp);
			fp = NULL;
		}

		mrb_gc_arena_restore(mrb, mrb_ai);

		mrb_nanogl_gem_final(mrb);
		mrb_close(mrb);
	}
}

void start(void)
{
	if (mrb != NULL && fp != NULL && c != NULL) {
		mrb_load_file_cxt(mrb, fp, c);
		check_exc();
	}
}
	
