/*                                                                -*- C -*-
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Stig S�ther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define CONFIGURE_COMMAND " '/BuildRoot/Library/Caches/com.apple.xbs/Binaries/apache_mod_php/install/TempContent/Objects/php/configure'  '--prefix=/usr' '--mandir=/usr/share/man' '--infodir=/usr/share/info' '--disable-dependency-tracking' '--sysconfdir=/private/etc' '--with-libdir=lib' '--enable-cli' '--with-iconv=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-config-file-path=/etc' '--with-libxml-dir=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-openssl=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr/local/libressl' '--with-kerberos=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-zlib=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-bcmath' '--with-bz2=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-calendar' '--disable-cgi' '--with-curl=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-dba' '--with-ndbm=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-exif' '--enable-fpm' '--enable-ftp' '--with-gd' '--with-png-dir=/usr/local' '--with-jpeg-dir=/usr/local' '--enable-gd-native-ttf' '--with-icu-dir=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-ldap=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-ldap-sasl=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-libedit=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-mbstring' '--enable-mbregex' '--with-mysqli=mysqlnd' '--with-pcre-regex=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--without-pcre-jit' '--with-pdo-pgsql=/Applications/Xcode.app/Contents/Developer/Toolchains/OSX10.13.xctoolchain/usr/local/bin' '--with-pgsql=/Applications/Xcode.app/Contents/Developer/Toolchains/OSX10.13.xctoolchain/local/bin' '--without-pear' '--with-pear=no' '--with-pdo-mysql=mysqlnd' '--with-mysql-sock=/var/mysql/mysql.sock' '--disable-phpdbg' '--with-readline=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-shmop' '--with-snmp=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-soap' '--enable-sockets' '--enable-sysvmsg' '--enable-sysvsem' '--enable-sysvshm' '--with-tidy=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--enable-wddx' '--with-xmlrpc' '--with-iconv-dir=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-xsl=/BuildRoot/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.Internal.sdk/usr' '--with-apxs2=/Applications/Xcode.app/Contents/Developer/Toolchains/OSX10.13.xctoolchain/usr/local/bin/apxs' '--enable-zip' 'YACC=/BuildRoot/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/bison'"
#define PHP_ADA_INCLUDE		""
#define PHP_ADA_LFLAGS		""
#define PHP_ADA_LIBS		""
#define PHP_APACHE_INCLUDE	""
#define PHP_APACHE_TARGET	""
#define PHP_FHTTPD_INCLUDE      ""
#define PHP_FHTTPD_LIB          ""
#define PHP_FHTTPD_TARGET       ""
#define PHP_CFLAGS		"$(CFLAGS_CLEAN) -prefer-non-pic -static"
#define PHP_DBASE_LIB		""
#define PHP_BUILD_DEBUG		""
#define PHP_GDBM_INCLUDE	""
#define PHP_IBASE_INCLUDE	""
#define PHP_IBASE_LFLAGS	""
#define PHP_IBASE_LIBS		""
#define PHP_IFX_INCLUDE		""
#define PHP_IFX_LFLAGS		""
#define PHP_IFX_LIBS		""
#define PHP_INSTALL_IT		"$(mkinstalldirs) '$(INSTALL_ROOT)/usr/libexec/apache2' &&                 $(mkinstalldirs) '$(INSTALL_ROOT)/private/etc/apache2' &&                  /Applications/Xcode.app/Contents/Developer/Toolchains/OSX10.13.xctoolchain/usr/local/bin/apxs -S LIBEXECDIR='$(INSTALL_ROOT)/usr/libexec/apache2'                        -S SYSCONFDIR='$(INSTALL_ROOT)/private/etc/apache2'                        -i -n php7 libs/libphp7.so"
#define PHP_IODBC_INCLUDE	""
#define PHP_IODBC_LFLAGS	""
#define PHP_IODBC_LIBS		""
#define PHP_MSQL_INCLUDE	""
#define PHP_MSQL_LFLAGS		""
#define PHP_MSQL_LIBS		""
#define PHP_MYSQL_INCLUDE	"@MYSQL_INCLUDE@"
#define PHP_MYSQL_LIBS		"@MYSQL_LIBS@"
#define PHP_MYSQL_TYPE		"@MYSQL_MODULE_TYPE@"
#define PHP_ODBC_INCLUDE	""
#define PHP_ODBC_LFLAGS		""
#define PHP_ODBC_LIBS		""
#define PHP_ODBC_TYPE		""
#define PHP_OCI8_SHARED_LIBADD 	""
#define PHP_OCI8_DIR			""
#define PHP_OCI8_ORACLE_VERSION		""
#define PHP_ORACLE_SHARED_LIBADD 	"@ORACLE_SHARED_LIBADD@"
#define PHP_ORACLE_DIR				"@ORACLE_DIR@"
#define PHP_ORACLE_VERSION			"@ORACLE_VERSION@"
#define PHP_PGSQL_INCLUDE	""
#define PHP_PGSQL_LFLAGS	""
#define PHP_PGSQL_LIBS		""
#define PHP_PROG_SENDMAIL	"/usr/sbin/sendmail"
#define PHP_SOLID_INCLUDE	""
#define PHP_SOLID_LIBS		""
#define PHP_EMPRESS_INCLUDE	""
#define PHP_EMPRESS_LIBS	""
#define PHP_SYBASE_INCLUDE	""
#define PHP_SYBASE_LFLAGS	""
#define PHP_SYBASE_LIBS		""
#define PHP_DBM_TYPE		""
#define PHP_DBM_LIB		""
#define PHP_LDAP_LFLAGS		""
#define PHP_LDAP_INCLUDE	""
#define PHP_LDAP_LIBS		""
#define PHP_BIRDSTEP_INCLUDE     ""
#define PHP_BIRDSTEP_LIBS        ""
#define PEAR_INSTALLDIR         ""
#define PHP_INCLUDE_PATH	".:"
#define PHP_EXTENSION_DIR       "/usr/lib/php/extensions/no-debug-non-zts-20160303"
#define PHP_PREFIX              "/usr"
#define PHP_BINDIR              "/usr/bin"
#define PHP_SBINDIR             "/usr/sbin"
#define PHP_MANDIR              "/usr/share/man"
#define PHP_LIBDIR              "/usr/lib/php"
#define PHP_DATADIR             "/usr/share/php"
#define PHP_SYSCONFDIR          "/private/etc"
#define PHP_LOCALSTATEDIR       "/usr/var"
#define PHP_CONFIG_FILE_PATH    "/etc"
#define PHP_CONFIG_FILE_SCAN_DIR    ""
#define PHP_SHLIB_SUFFIX        "so"
