#include <stdio.h>
#include <sqlite3.h>
#include "../include/hash_map.h"
#include "../include/map_iterator.h"
#include "../include/repository.h"

sqlite3 *db = NULL;

bool init_data() {
    // Attempt to open database connection
    int rc = sqlite3_open("sparse_calc.sqlite", &db);

    // Return false if fails
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    // Create table with sql
    const char* sql_create = 
        "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS matrices ("
        "   name STRING PRIMARY KEY,"
        "   rows INTEGER NOT NULL,"
        "   cols INTEGER NOT NULL,"
        "   scalar_val REAL NOT NULL)"
        "CREATE TABLE IF NOT EXISTS matrix_vals ("
        "   name STRING PRIMARY KEY,"
        "   row INTEGER PRIMARY KEY"
        "   col INTEGER PRIMARY KEY"
        "   val REAL NOT NULL"
        "   FOREIGN KEY(name) REFERENCES matrices(name));";

    // Attempt to Execute table creation
    char *err_msg = NULL;
    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);

    // Return false if fails
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return false;
    }

    return true;
}

bool repo_is_unique(char* name) {
    if(!db) // Initialize database if not done yet
        init_data();

    return true;
}

bool delete_matrix_vals(char* name) {
    return true;
}

bool insert_matrix_vals(char* name, Matrix* matrix) {
    if(!db) // Initialize database if not done yet
        init_data();
    
    // Attempt to delete existing values for matrix
    bool delete_result = delete_matrix_vals(name);

    // Return false if delete fails
    if(!delete_result)
        return false;

    if(!db) // Initialize database if not done yet
        init_data();

    // Create statement with placeholders
    const char* sql = "INSERT OR REPLACE INTO matrix_vals (name, row, col, val) VALUES (? ? ? ?)";
    sqlite3_stmt* stmt;
    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    // Return false if error
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    MapIterator map_it = map_iterator_create(matrix->vals);
    while(map_iterator_has_next(&map_it)) {
        // Get next non-zero value
        int row, col;
        double val;
        map_iterator_next(&map_it, &row, &col, &val);

        // Bind element to statement placeholder
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, row);
        sqlite3_bind_int(stmt, 3, col);
        sqlite3_bind_double(stmt, 4, val);

        // Attempt to execute insert
        sqlite3_step(stmt);

        // Return false if insert fails
        if(rc != SQLITE_DONE) {
            fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return false;
        }

        // Reset stmt and clear bindings
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }

    // Free allocated memory for statement
    sqlite3_finalize(stmt);

    return true;    
}

bool repo_matrix_save(char* name, Matrix* matrix) {
    if(!db) // Initialize database if not done yet
        init_data();

    // Create statement with placeholders
    const char* sql = "INSERT OR REPLACE INTO matrices (name, rows, cols, scalar_val) VALUES (? ? ? ?)";
    sqlite3_stmt* stmt;

    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    // Return false if error
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    // Bind matrix data to statement placeholders
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, matrix->rows);
    sqlite3_bind_int(stmt, 3, matrix->cols);
    sqlite3_bind_double(stmt, 4, matrix->scalar_val);

    // Attempt to execute the statement
    rc = sqlite3_step(stmt);

    // Return false if fails
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    // Free memory allocated for statement
    sqlite3_finalize(stmt);

    // Return result of inserting matrix values
    return insert_matrix_vals(name, matrix);
}