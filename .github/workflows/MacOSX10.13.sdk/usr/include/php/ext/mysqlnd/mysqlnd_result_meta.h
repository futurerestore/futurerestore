/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Johannes Schl�ter <johannes@php.net>                        |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_RESULT_META_H
#define MYSQLND_RESULT_META_H

PHPAPI MYSQLND_RES_METADATA * mysqlnd_result_meta_init(unsigned int field_count, zend_bool persistent);
PHPAPI struct st_mysqlnd_res_meta_methods * mysqlnd_result_metadata_get_methods();
PHPAPI void ** _mysqlnd_plugin_get_plugin_result_metadata_data(const MYSQLND_RES_METADATA * meta, unsigned int plugin_id);

#endif /* MYSQLND_RESULT_META_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
