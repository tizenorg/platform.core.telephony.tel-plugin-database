/*
 * tel-plugin-database
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: DongHoo Park <donghoo.park@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <glib.h>
#include <db-util.h>

#include <tcore.h>
#include <plugin.h>
#include <storage.h>

#ifndef PLUGIN_VERSION
#define PLUGIN_VERSION 1
#endif

static void* create_handle(Storage *strg, const char *path)
{
	int rv = 0;
	sqlite3 *handle = NULL;

	rv = db_util_open(path, &handle, 0);
	if (rv != SQLITE_OK) {
		err("fail to connect database err(%d)", rv);
		return NULL;
	}

	dbg("connected to %s", path);
	return handle;
}

static gboolean remove_handle(Storage *strg, void *handle)
{
	if (!handle)
		return FALSE;

	db_util_close(handle);
	//free(handle);

	dbg("disconnected from database");
	return TRUE;
}

static gboolean update_query_database(Storage *strg, void *handle, const char *query, GHashTable *in_param)
{
	int rv = 0;
	sqlite3_stmt* stmt = NULL;
	char szQuery[1000+1];	// +1 is for NULL Termination Character '\0'

	GHashTableIter iter;
	gpointer key, value;

	dbg("update query");

	memset(szQuery, '\0', 1001);
	strncpy(szQuery, query, 1000);

	rv = sqlite3_prepare_v2(handle, szQuery, strlen(szQuery), &stmt, NULL);
	if (rv != SQLITE_OK) {
		err("fail to connect to table (%d)", rv);
		return FALSE;
	}

	if(in_param){
		g_hash_table_iter_init(&iter, in_param);
		while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
			dbg("key(%s), value(%s)", (const char*)key, (const char*)value);

			if (!value || g_strcmp0((const char*) value, "") == 0) {
				dbg("bind null");
				rv = sqlite3_bind_null(stmt, atoi((const char*) key));
			}
			else {
				dbg("bind value");
				rv = sqlite3_bind_text(stmt, atoi((const char*) key), (const char*) value, strlen((const char*) value),
						SQLITE_STATIC);
			}

			if (rv != SQLITE_OK) {
				dbg("fail to bind data (%d)", rv);
				return FALSE;
			}
		}
	}

	rv = sqlite3_step(stmt);
	dbg("update query executed (%d)", rv);
	sqlite3_finalize(stmt);

	if (rv != SQLITE_DONE) {
		return FALSE;
	}

	return TRUE;
}

static gboolean read_query_database(Storage *strg, void *handle, const char *query, GHashTable *in_param,
		GHashTable *out_param, int out_param_cnt)
{
	int rv = 0, local_index = 0, outter_index = 0;
	sqlite3_stmt* stmt = NULL;
	char szQuery[5000+1];	// +1 is for NULL Termination Character '\0'

	GHashTableIter iter;
	gpointer key, value;

	dbg("read query");

	memset(szQuery, '\0', 5001);
	strncpy(szQuery, query, 5000);

	rv = sqlite3_prepare_v2(handle, szQuery, strlen(szQuery), &stmt, NULL);
	if (rv != SQLITE_OK) {
		err("fail to connect to table (%d)", rv);
		return FALSE;
	}

	if (in_param) {
		g_hash_table_iter_init(&iter, in_param);
		while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
			dbg("key(%s), value(%s)", (const char*)key, (const char*)value);

			if (!value || g_strcmp0((const char*) value, "") == 0) {
				dbg("bind null");
				rv = sqlite3_bind_null(stmt, atoi((const char*) key));
			}
			else {
				dbg("bind value");
				rv = sqlite3_bind_text(stmt, atoi((const char*) key), (const char*) value, strlen((const char*) value),
						SQLITE_STATIC);
			}

			if (rv != SQLITE_OK) {
				dbg("fail to bind data (%d)", rv);
				return FALSE;
			}
		}
	}

	rv = sqlite3_step(stmt);
	dbg("read query executed (%d)", rv);

	while (rv == SQLITE_ROW) {

		char tmp_key_outter[10];
		GHashTable *out_param_data;

		out_param_data = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);

		for (local_index = 0; local_index < out_param_cnt; local_index++) {
			char tmp_key[10];
			const unsigned char *tmp;
			tmp = sqlite3_column_text(stmt, local_index);
			snprintf(tmp_key, sizeof(tmp_key), "%d", local_index);
			g_hash_table_insert(out_param_data, g_strdup(tmp_key), g_strdup((const gchar *) tmp));
		}

		snprintf(tmp_key_outter, sizeof(tmp_key_outter), "%d", outter_index);
		g_hash_table_insert(out_param, g_strdup(tmp_key_outter), out_param_data);
		outter_index++;
		rv = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);
	return TRUE;
}

static gboolean insert_query_database(Storage *strg, void *handle, const char *query, GHashTable *in_param)
{
	int rv = 0;
	sqlite3_stmt* stmt = NULL;
	char szQuery[5000+1];	// +1 is for NULL Termination Character '\0'

	GHashTableIter iter;
	gpointer key, value;
	dbg("insert query");

	memset(szQuery, '\0', 5001);
	strncpy(szQuery, query, 5000);

	rv = sqlite3_prepare_v2(handle, szQuery, strlen(szQuery), &stmt, NULL);
	if (rv != SQLITE_OK) {
		err("fail to connect to table (%d)", rv);
		return FALSE;
	}

	if(in_param){
		g_hash_table_iter_init(&iter, in_param);
		while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
			dbg("key(%s), value(%s)", (const char*)key, (const char*)value);

			if (!value || g_strcmp0((const char*) value, "") == 0) {
				dbg("bind null");
				rv = sqlite3_bind_null(stmt, atoi((const char*) key));
			}
			else {
				dbg("bind value");
				rv = sqlite3_bind_text(stmt, atoi((const char*) key), (const char*) value, strlen((const char*) value),
						SQLITE_STATIC);
			}

			if (rv != SQLITE_OK) {
				dbg("fail to bind data (%d)", rv);
				return FALSE;
			}
		}
	}

	rv = sqlite3_step(stmt);
	dbg("insert query executed (%d)", rv);
	sqlite3_finalize(stmt);

	if (rv != SQLITE_DONE) {
		return FALSE;
	}
	return TRUE;
}

static gboolean remove_query_database(Storage *strg, void *handle, const char *query, GHashTable *in_param)
{
	int rv = 0;
	sqlite3_stmt* stmt = NULL;
	char szQuery[1000+1];	// +1 is for NULL Termination Character '\0'

	GHashTableIter iter;
	gpointer key, value;
	dbg("remove query");

	memset(szQuery, '\0', 1001);
	strncpy(szQuery, query, 1000);

	rv = sqlite3_prepare_v2(handle, szQuery, strlen(szQuery), &stmt, NULL);
	if (rv != SQLITE_OK) {
		err("fail to connect to table (%d)", rv);
		return FALSE;
	}

	if(in_param){
		g_hash_table_iter_init(&iter, in_param);
		while (g_hash_table_iter_next(&iter, &key, &value) == TRUE) {
			dbg("key(%s), value(%s)", (const char*)key, (const char*)value);

			if (!value || g_strcmp0((const char*) value, "") == 0) {
				dbg("bind null");
				rv = sqlite3_bind_null(stmt, atoi((const char*) key));
			}
			else {
				dbg("bind value");
				rv = sqlite3_bind_text(stmt, atoi((const char*) key), (const char*) value, strlen((const char*) value),
						SQLITE_STATIC);
			}

			if (rv != SQLITE_OK) {
				dbg("fail to bind data (%d)", rv);
				return FALSE;
			}
		}
	}

	rv = sqlite3_step(stmt);
	dbg("remove query executed (%d)", rv);
	sqlite3_finalize(stmt);

	if (rv != SQLITE_DONE) {
		return FALSE;
	}

	return TRUE;
}

static struct storage_operations ops = {
	.create_handle = create_handle,
	.remove_handle = remove_handle,
	.update_query_database = update_query_database,
	.read_query_database = read_query_database,
	.insert_query_database = insert_query_database,
	.remove_query_database = remove_query_database,
};

static gboolean on_load()
{
	dbg("i'm load!");
	return TRUE;
}

static gboolean on_init(TcorePlugin *p)
{
	if (!p)
		return FALSE;

	tcore_storage_new(p, "database", &ops);

	dbg("finish to initialize database plug-in");
	return TRUE;
}

static void on_unload(TcorePlugin *p)
{
	dbg("i'm unload!");
	return;
}

EXPORT_API struct tcore_plugin_define_desc plugin_define_desc =
{
	.name = "DATABASE",
	.priority = TCORE_PLUGIN_PRIORITY_HIGH -1,
	.version = PLUGIN_VERSION,
	.load = on_load,
	.init = on_init,
	.unload = on_unload
};
