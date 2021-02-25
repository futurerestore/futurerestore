# frozen-string-literal: false
# This file was created by mkconfig.rb when ruby was built.  It contains
# build information for ruby which is used e.g. by mkmf to build
# compatible native extensions.  Any changes made to this file will be
# lost the next time ruby is built.

module RbConfig
  RUBY_VERSION.start_with?("2.3.") or
    raise "ruby lib version (2.3.3) doesn't match executable version (#{RUBY_VERSION})"

  TOPDIR = File.dirname(__FILE__).chomp!("/lib/ruby/2.3.0/universal-darwin17")
  DESTDIR = '' unless defined? DESTDIR
  arch_flag = ENV['ARCHFLAGS'] || ((e = ENV['RC_ARCHS']) && e.split.uniq.map {|a| "-arch #{a}"}.join(' '))
  universal = " x86_64=x86_64 i386=i386"
  CONFIG = {}
  CONFIG["DESTDIR"] = DESTDIR
  CONFIG["MAJOR"] = "2"
  CONFIG["MINOR"] = "3"
  CONFIG["TEENY"] = "0"
  CONFIG["PATCHLEVEL"] = "222"
  CONFIG["INSTALL"] = '/usr/bin/install -c'
  CONFIG["EXEEXT"] = ""
  CONFIG["prefix"] = (TOPDIR || DESTDIR + "/System/Library/Frameworks/Ruby.framework/Versions/2.3/usr")
  CONFIG["ruby_install_name"] = "$(RUBY_BASE_NAME)"
  CONFIG["RUBY_INSTALL_NAME"] = "$(RUBY_BASE_NAME)"
  CONFIG["RUBY_SO_NAME"] = "$(RUBY_BASE_NAME).$(MAJOR).$(MINOR).$(TEENY)"
  CONFIG["exec"] = "exec"
  CONFIG["ruby_pc"] = "ruby-2.3.pc"
  CONFIG["PACKAGE"] = "ruby"
  CONFIG["BUILTIN_TRANSSRCS"] = " enc/trans/newline.c"
  CONFIG["USE_RUBYGEMS"] = "YES"
  CONFIG["MANTYPE"] = "doc"
  CONFIG["NROFF"] = "/usr/bin/nroff"
  CONFIG["vendorarchhdrdir"] = "$(vendorhdrdir)/$(sitearch)"
  CONFIG["sitearchhdrdir"] = "$(sitehdrdir)/$(sitearch)"
  CONFIG["rubyarchhdrdir"] = "$(rubyhdrdir)/$(arch)"
  CONFIG["vendorhdrdir"] = "$(rubyhdrdir)/vendor_ruby"
  CONFIG["sitehdrdir"] = "$(rubyhdrdir)/site_ruby"
  CONFIG["rubyhdrdir"] = "$(includedir)/$(RUBY_VERSION_NAME)"
  CONFIG["RUBY_SEARCH_PATH"] = ""
  CONFIG["UNIVERSAL_INTS"] = "'long long' long int short"
  CONFIG["UNIVERSAL_ARCHNAMES"] = universal
  CONFIG["configure_args"] = " '--prefix=/usr' '--mandir=/usr/share/man' '--infodir=/usr/share/info' '--disable-dependency-tracking' '--prefix=/System/Library/Frameworks/Ruby.framework/Versions/2.3/usr' '--sysconfdir=/Library/Ruby/Site' '--with-sitedir=/Library/Ruby/Site' '--enable-shared' '--with-arch=x86_64,i386' '--without-ext=tk' 'ac_cv_func_getcontext=no' 'ac_cv_func_setcontext=no' 'ac_cv_func_utimensat=no' 'ac_cv_c_compiler_gnu=no' 'ac_cv_header_net_if_h=yes' 'av_cv_header_ifaddrs_h=yes' 'rb_cv_pri_prefix_long_long=ll' 'ac_cv_sizeof_struct_stat_st_size=SIZEOF_OFF_T' 'ac_cv_sizeof_struct_stat_st_blocks=SIZEOF_INT64_T' 'ac_cv_sizeof_struct_stat_st_ino=SIZEOF_UINT64_T' 'CC=/BuildRoot/Applications/Xcode.app/Contents/Developer/Toolchains/OSX10.13.xctoolchain/usr/bin/cc' 'CFLAGS=-arch x86_64 -arch i386 -g -Os -pipe -DHAVE_GCC_ATOMIC_BUILTINS -iwithsysroot /usr/local/libressl/include' 'LDFLAGS=-arch x86_64 -arch i386            -L /BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr/local/libressl/lib' 'CXX=/BuildRoot/Applications/Xcode.app/Contents/Developer/Toolchains/OSX10.13.xctoolchain/usr/bin/c++' 'CXXFLAGS=-arch x86_64 -arch i386 -g -Os -pipe '"
  CONFIG["CONFIGURE"] = "configure"
  CONFIG["vendorarchdir"] = "$(vendorlibdir)/$(sitearch)"
  CONFIG["vendorlibdir"] = "$(vendordir)/$(ruby_version)"
  CONFIG["vendordir"] = "$(rubylibprefix)/vendor_ruby"
  CONFIG["sitearchdir"] = "$(sitelibdir)/$(sitearch)"
  CONFIG["sitelibdir"] = "$(sitedir)/$(ruby_version)"
  CONFIG["sitedir"] = "$(DESTDIR)/Library/Ruby/Site"
  CONFIG["rubyarchdir"] = "$(rubylibdir)/$(arch)"
  CONFIG["rubylibdir"] = "$(rubylibprefix)/$(ruby_version)"
  CONFIG["ruby_version"] = "2.3.0"
  CONFIG["sitearch"] = "$(arch)"
  CONFIG["arch"] = "universal-darwin17"
  CONFIG["sitearchincludedir"] = "$(includedir)/$(sitearch)"
  CONFIG["archincludedir"] = "$(includedir)/$(arch)"
  CONFIG["sitearchlibdir"] = "$(libdir)/$(sitearch)"
  CONFIG["archlibdir"] = "$(libdir)/$(arch)"
  CONFIG["libdirname"] = "libdir"
  CONFIG["RUBY_EXEC_PREFIX"] = "/System/Library/Frameworks/Ruby.framework/Versions/2.3/usr"
  CONFIG["RUBY_LIB_VERSION"] = ""
  CONFIG["RUBY_LIB_VERSION_STYLE"] = "3\t/* full */"
  CONFIG["RI_BASE_NAME"] = "ri"
  CONFIG["ridir"] = "$(datarootdir)/$(RI_BASE_NAME)"
  CONFIG["rubysitearchprefix"] = "$(rubylibprefix)/$(sitearch)"
  CONFIG["rubyarchprefix"] = "$(rubylibprefix)/$(arch)"
  CONFIG["MAKEFILES"] = "Makefile GNUmakefile"
  CONFIG["PLATFORM_DIR"] = ""
  CONFIG["THREAD_MODEL"] = "pthread"
  CONFIG["SYMBOL_PREFIX"] = "_"
  CONFIG["EXPORT_PREFIX"] = ""
  CONFIG["COMMON_HEADERS"] = ""
  CONFIG["COMMON_MACROS"] = ""
  CONFIG["COMMON_LIBS"] = ""
  CONFIG["MAINLIBS"] = ""
  CONFIG["ENABLE_SHARED"] = "yes"
  CONFIG["DLDLIBS"] = ""
  CONFIG["SOLIBS"] = "$(LIBS)"
  CONFIG["LIBRUBYARG_SHARED"] = "-l$(RUBY_SO_NAME)"
  CONFIG["LIBRUBYARG_STATIC"] = "-l$(RUBY_SO_NAME)-static -framework CoreFoundation"
  CONFIG["LIBRUBYARG"] = "$(LIBRUBYARG_SHARED)"
  CONFIG["LIBRUBY"] = "$(LIBRUBY_SO)"
  CONFIG["LIBRUBY_ALIASES"] = "lib$(RUBY_BASE_NAME).$(MAJOR).$(MINOR).dylib lib$(RUBY_INSTALL_NAME).dylib"
  CONFIG["LIBRUBY_SO"] = "lib$(RUBY_SO_NAME).dylib"
  CONFIG["LIBRUBY_A"] = "lib$(RUBY_SO_NAME)-static.a"
  CONFIG["RUBYW_INSTALL_NAME"] = ""
  CONFIG["rubyw_install_name"] = ""
  CONFIG["EXTDLDFLAGS"] = ""
  CONFIG["EXTLDFLAGS"] = ""
  CONFIG["strict_warnflags"] = ""
  CONFIG["warnflags"] = ""
  CONFIG["debugflags"] = "-g"
  CONFIG["optflags"] = ""
  CONFIG["NULLCMD"] = ":"
  CONFIG["DLNOBJ"] = "dln.o"
  CONFIG["EXECUTABLE_EXTS"] = ""
  CONFIG["ARCHFILE"] = ""
  CONFIG["LIBRUBY_RELATIVE"] = "no"
  CONFIG["EXTOUT"] = ".ext"
  CONFIG["PREP"] = "miniruby$(EXEEXT)"
  CONFIG["CROSS_COMPILING"] = "no"
  CONFIG["TEST_RUNNABLE"] = "yes"
  CONFIG["rubylibprefix"] = "$(libdir)/$(RUBY_BASE_NAME)"
  CONFIG["setup"] = "Setup"
  CONFIG["ENCSTATIC"] = ""
  CONFIG["EXTSTATIC"] = ""
  CONFIG["STRIP"] = "strip -A -n"
  CONFIG["TRY_LINK"] = ""
  CONFIG["PRELOADENV"] = "DYLD_INSERT_LIBRARIES"
  CONFIG["LIBPATHENV"] = "DYLD_LIBRARY_PATH"
  CONFIG["RPATHFLAG"] = ""
  CONFIG["LIBPATHFLAG"] = " -L%s"
  CONFIG["LINK_SO"] = "\n$(POSTLINK)"
  CONFIG["ASMEXT"] = "S"
  CONFIG["LIBEXT"] = "a"
  CONFIG["DLEXT2"] = ""
  CONFIG["DLEXT"] = "bundle"
  CONFIG["LDSHAREDXX"] = "$(CXX) -dynamic -bundle"
  CONFIG["LDSHARED"] = "$(CC) -dynamic -bundle"
  CONFIG["CCDLFLAGS"] = ""
  CONFIG["STATIC"] = ""
  CONFIG["ARCH_FLAG"] = arch_flag || " -arch x86_64 -arch i386"
  CONFIG["DLDFLAGS"] = "-Wl,-undefined,dynamic_lookup -Wl,-multiply_defined,suppress"
  CONFIG["ALLOCA"] = ""
  CONFIG["codesign"] = "codesign"
  CONFIG["POSTLINK"] = "test -z '$(RUBY_CODESIGN)' || codesign -s '$(RUBY_CODESIGN)' -f $@"
  CONFIG["WERRORFLAG"] = ""
  CONFIG["CHDIR"] = "cd -P"
  CONFIG["RMALL"] = "rm -fr"
  CONFIG["RMDIRS"] = "rmdir -p"
  CONFIG["RMDIR"] = "rmdir"
  CONFIG["CP"] = "cp"
  CONFIG["RM"] = "rm -f"
  CONFIG["PKG_CONFIG"] = ""
  CONFIG["PYTHON"] = ""
  CONFIG["DOXYGEN"] = ""
  CONFIG["DOT"] = ""
  CONFIG["MAKEDIRS"] = "mkdir -p"
  CONFIG["MKDIR_P"] = "mkdir -p"
  CONFIG["INSTALL_DATA"] = "$(INSTALL) -m 644"
  CONFIG["INSTALL_SCRIPT"] = "$(INSTALL)"
  CONFIG["INSTALL_PROGRAM"] = "$(INSTALL)"
  CONFIG["SET_MAKE"] = ""
  CONFIG["LN_S"] = "ln -s"
  CONFIG["NM"] = "nm"
  CONFIG["DLLWRAP"] = ""
  CONFIG["WINDRES"] = ""
  CONFIG["OBJCOPY"] = ""
  CONFIG["OBJDUMP"] = "objdump"
  CONFIG["ASFLAGS"] = ""
  CONFIG["AS"] = "as"
  CONFIG["AR"] = "ar"
  CONFIG["RANLIB"] = "ranlib"
  CONFIG["try_header"] = "try_compile"
  CONFIG["CC_VERSION"] = "$(CC) -v"
  CONFIG["COUTFLAG"] = "-o "
  CONFIG["OUTFLAG"] = "-o "
  CONFIG["CPPOUTFILE"] = "-o conftest.i"
  CONFIG["GNU_LD"] = "no"
  CONFIG["LD"] = "ld"
  CONFIG["GCC"] = ""
  CONFIG["EGREP"] = "/usr/bin/grep -E"
  CONFIG["GREP"] = "/usr/bin/grep"
  CONFIG["CPP"] = "$(CC) -E"
  CONFIG["CXXFLAGS"] = " -g -Os -pipe "
  CONFIG["OBJEXT"] = "o"
  CONFIG["CPPFLAGS"] = "-D_XOPEN_SOURCE -D_DARWIN_C_SOURCE -D_DARWIN_UNLIMITED_SELECT -D_REENTRANT $(DEFS) $(cppflags)"
  CONFIG["LDFLAGS"] = "-L.             -L /BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr/local/libressl/lib -L/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr/local/lib"
  CONFIG["CFLAGS"] = " -g -Os -pipe -DHAVE_GCC_ATOMIC_BUILTINS -iwithsysroot /usr/local/libressl/include"
  CONFIG["CXX"] = "xcrun clang++"
  CONFIG["CC"] = "xcrun clang"
  CONFIG["NACL_LIB_PATH"] = ""
  CONFIG["NACL_SDK_VARIANT"] = ""
  CONFIG["NACL_SDK_ROOT"] = ""
  CONFIG["NACL_TOOLCHAIN"] = ""
  CONFIG["target_os"] = "darwin17"
  CONFIG["target_vendor"] = "apple"
  CONFIG["target_cpu"] = "universal"
  CONFIG["target"] = "universal-apple-darwin17"
  CONFIG["host_os"] = "darwin17"
  CONFIG["host_vendor"] = "apple"
  CONFIG["host_cpu"] = "x86_64"
  CONFIG["host"] = "x86_64-apple-darwin17"
  CONFIG["RUBY_VERSION_NAME"] = "$(RUBY_BASE_NAME)-$(ruby_version)"
  CONFIG["RUBYW_BASE_NAME"] = "rubyw"
  CONFIG["RUBY_BASE_NAME"] = "ruby"
  CONFIG["build_os"] = "darwin17"
  CONFIG["build_vendor"] = "apple"
  CONFIG["build_cpu"] = "x86_64"
  CONFIG["build"] = "x86_64-apple-darwin17"
  CONFIG["RUBY_PROGRAM_VERSION"] = "2.3.3"
  CONFIG["cxxflags"] = " $(optflags) $(debugflags) $(warnflags)"
  CONFIG["cppflags"] = ""
  CONFIG["cflags"] = " $(optflags) $(debugflags) $(warnflags)"
  CONFIG["target_alias"] = ""
  CONFIG["host_alias"] = ""
  CONFIG["build_alias"] = ""
  CONFIG["LIBS"] = "-lpthread -ldl -lobjc"
  CONFIG["ECHO_T"] = ""
  CONFIG["ECHO_N"] = ""
  CONFIG["ECHO_C"] = "\\\\c"
  CONFIG["DEFS"] = ""
  CONFIG["mandir"] = "$(DESTDIR)/usr/share/man"
  CONFIG["localedir"] = "$(datarootdir)/locale"
  CONFIG["libdir"] = "$(exec_prefix)/lib"
  CONFIG["psdir"] = "$(docdir)"
  CONFIG["pdfdir"] = "$(docdir)"
  CONFIG["dvidir"] = "$(docdir)"
  CONFIG["htmldir"] = "$(docdir)"
  CONFIG["infodir"] = "$(DESTDIR)/usr/share/info"
  CONFIG["docdir"] = "$(datarootdir)/doc/$(PACKAGE)"
  CONFIG["oldincludedir"] = "/usr/include"
  CONFIG["includedir"] = (ENV['SDKROOT'] || (File.exist?(File.join(CONFIG['prefix'],'include')) ? '' : %x(xcode-select --print-path >/dev/null 2>&1 && xcrun --sdk macosx --show-sdk-path 2>/dev/null).chomp)) + "$(prefix)/include"
  CONFIG["localstatedir"] = "$(prefix)/var"
  CONFIG["sharedstatedir"] = "$(prefix)/com"
  CONFIG["sysconfdir"] = "$(DESTDIR)/Library/Ruby/Site"
  CONFIG["datadir"] = "$(datarootdir)"
  CONFIG["datarootdir"] = "$(prefix)/share"
  CONFIG["libexecdir"] = "$(exec_prefix)/libexec"
  CONFIG["sbindir"] = "$(exec_prefix)/sbin"
  CONFIG["bindir"] = "$(exec_prefix)/bin"
  CONFIG["exec_prefix"] = "$(prefix)"
  CONFIG["PACKAGE_URL"] = ""
  CONFIG["PACKAGE_BUGREPORT"] = ""
  CONFIG["PACKAGE_STRING"] = ""
  CONFIG["PACKAGE_VERSION"] = ""
  CONFIG["PACKAGE_TARNAME"] = ""
  CONFIG["PACKAGE_NAME"] = ""
  CONFIG["PATH_SEPARATOR"] = ":"
  CONFIG["SHELL"] = "/bin/sh"
  CONFIG["SDKROOT"] = ENV["SDKROOT"] || "" # don't run xcrun everytime, usually useless.
  CONFIG["archdir"] = "$(rubyarchdir)"
  CONFIG["topdir"] = File.dirname(__FILE__)
  MAKEFILE_CONFIG = {}
  CONFIG.each{|k,v| MAKEFILE_CONFIG[k] = v.dup}
  def RbConfig::expand(val, config = CONFIG)
    newval = val.gsub(/\$\$|\$\(([^()]+)\)|\$\{([^{}]+)\}/) {
      var = $&
      if !(v = $1 || $2)
	'$'
      elsif key = config[v = v[/\A[^:]+(?=(?::(.*?)=(.*))?\z)/]]
	pat, sub = $1, $2
	config[v] = false
	config[v] = RbConfig::expand(key, config)
	key = key.gsub(/#{Regexp.quote(pat)}(?=\s|\z)/n) {sub} if pat
	key
      else
	var
      end
    }
    val.replace(newval) unless newval == val
    val
  end
  CONFIG.each_value do |val|
    RbConfig::expand(val)
  end

  # returns the absolute pathname of the ruby command.
  def RbConfig.ruby
    File.join(
      RbConfig::CONFIG["bindir"],
      RbConfig::CONFIG["ruby_install_name"] + RbConfig::CONFIG["EXEEXT"]
    )
  end
end
CROSS_COMPILING = nil unless defined? CROSS_COMPILING
RUBY_FRAMEWORK = true
RUBY_FRAMEWORK_VERSION = RbConfig::CONFIG['ruby_version']
APPLE_GEM_HOME = File.join(RbConfig::CONFIG['libdir'], 'ruby/gems', RbConfig::CONFIG['ruby_version'])
