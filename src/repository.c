#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "../include/hash_map.h"
#include "../include/map_iterator.h"
#include "../include/repository.h"

sqlite3* db = NULL;

bool exec_sql(const char *sql) {
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

bool exec_prepared_stmt(sqlite3_stmt *stmt) {
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    return true;
}


bool init_data() {
    // Attempt to open database connection
    int rc = sqlite3_open("sparse_calc.sqlite", &db);

    // Return false if fails
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return false;
    }

    // Create database schema if needed
    const char* pragma = "PRAGMA foreign_keys = ON;";
    const char* sql_create_matrices =
        "CREATE TABLE IF NOT EXISTS matrices ("
        "   name TEXT PRIMARY KEY,"
        "   rows INTEGER NOT NULL,"
        "   cols INTEGER NOT NULL,"
        "   scalar_val REAL NOT NULL);";
    
    const char* sql_create_matrix_vals =
        "CREATE TABLE IF NOT EXISTS matrix_vals ("
        "   name TEXT,"
        "   row INTEGER,"
        "   col INTEGER,"
        "   val REAL NOT NULL,"
        "   PRIMARY KEY(name, row, col),"
        "   FOREIGN KEY(name) REFERENCES matrices(name) ON DELETE CASCADE);";
    
    // Attempt to execute schema creation
    if(!exec_sql(pragma)) return false;
    if(!exec_sql(sql_create_matrices)) return false;
    if(!exec_sql(sql_create_matrix_vals)) return false;
    
    return true;
}

bool connect() {
    bool connected = true;

    if(!db)
        connected = init_data();

    return connected;
}

bool repo_is_unique(char* name) {
    if(!connect()) // Connect to database
        return false; // Connection failed

    return true;
}

bool delete_matrix_vals(char* name) {
    if(!connect()) // Connect to database
        return false; // Connection failed

    return true;
}


bool insert_matrix_vals(char* name, Matrix* matrix) {
    if(!connect()) // Connect to database
        return false; // Connection failed

    // Create statement with placeholders
    const char* sql = "INSERT OR REPLACE INTO matrix_vals (name, row, col, val) VALUES (?, ?, ?, ?)";
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

        #ifdef DBG
        printf("Values Inserted:\n");
        printf("Row: %d, Col: %d, Value: %.2f\n", row, col, val);
        #endif

        // Bind element to statement placeholder
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, row);
        sqlite3_bind_int(stmt, 3, col);
        sqlite3_bind_double(stmt, 4, val);

        // Return false if insert fails
        if(!exec_prepared_stmt(stmt)) {
            fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
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


bool repo_matrix_delete(char* name) {
    if(!connect()) // Connect to database
        return false; // Connection failed

    // Create query and statement
    const char *sql = "DELETE FROM matrices WHERE name = ?;";
    sqlite3_stmt *stmt;

    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    bool success = exec_prepared_stmt(stmt);
    sqlite3_finalize(stmt);

    return success;
}


bool repo_matrix_save(char* name, Matrix* matrix) {
    if(!connect()) // Connect to database
        return false; // Connection failed

    // Delete existing matrix to cascade delete vals
    repo_matrix_delete(name);

    // Create statement with placeholders
    const char* sql = "INSERT OR REPLACE INTO matrices (name, rows, cols, scalar_val) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    // Return false if error
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    // Bind matrix data to statement placeholders
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, matrix->rows);
    sqlite3_bind_int(stmt, 3, matrix->cols);
    sqlite3_bind_double(stmt, 4, matrix->scalar_val);

    // Execute matrix insert
    if(!exec_prepared_stmt(stmt))
        return false; // Insert failed

    // Free memory allocated for statement
    sqlite3_finalize(stmt);

    // Return result of inserting matrix values
    return insert_matrix_vals(name, matrix);
}


bool load_matrix_vals(char* name, Matrix* matrix) {
    if(!connect()) // Connect to database
        return false; // Connection failed
    
    // Create query and statement
    const char *sql = "SELECT row, col, val FROM matrix_vals WHERE name = ?;";
    sqlite3_stmt* stmt;

    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }

    // Bind matrix name to prepared statement
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    // Iterate result rows
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Get element from query results
        int row = sqlite3_column_int(stmt, 0);
        int col = sqlite3_column_int(stmt, 1);
        double val = sqlite3_column_double(stmt, 2);

        #ifdef DBG
        printf("Vals loaded:\n");
        printf("Row: %d, Col: %d, Value: %.2f\n", row, col, val);
        #endif
    
        matrix_set(matrix, row, col, val); // Add to matrix
    }

    if(rc != SQLITE_DONE)
        return false;

    // Free allocated memory for statement
    sqlite3_finalize(stmt);

    return true;
}

Matrix* repo_matrix_load(char* name) {
    if(!connect()) // Connect to database
        return NULL; // Connection failed

    // Create query and statement
    const char *sql = "SELECT rows, cols, scalar_val FROM matrices WHERE name = ? LIMIT 1;";
    sqlite3_stmt* stmt;

    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind matrix name to prepared statement
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt); // Execute query

    Matrix* matrix = NULL;

    if(rc == SQLITE_ROW) { // Query successful
        // Retrieve values from query
        int row = sqlite3_column_int(stmt, 0);
        int col = sqlite3_column_int(stmt, 1);
        int scalar_val = sqlite3_column_double(stmt, 2);

        // Instantiate matrix
        matrix = matrix_create(row, col);
        matrix->scalar_val = scalar_val;
    } else if(rc == SQLITE_DONE) { // Matrix does not exist
        printf("Matrix %s does not exist\n", name);
        return NULL;
    } else { // Query failed
        fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        
        return NULL;
    }

    // Free allocated memory for statement
    sqlite3_finalize(stmt);

    if(!load_matrix_vals(name, matrix))
        return NULL;

    return matrix;
}


// Print names and dimensions of all saved matrices
bool repo_list() {
    if(!connect()) // Connect to database
        return false; // Connection failed

    // Create query and statement
    const char *sql = "SELECT name, rows, cols FROM matrices;";
    sqlite3_stmt* stmt;

    // Attempt to prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }


    // Iterate result rows
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Get element from query results
        const unsigned char *name = sqlite3_column_text(stmt, 0);
        int rows = sqlite3_column_int(stmt, 1);
        int cols = sqlite3_column_int(stmt, 2);
        
        printf("%s\t(%d x %d)\n", name, rows, cols);
    }

    if(rc != SQLITE_DONE)
        return false;

    // Free allocated memory for statement
    sqlite3_finalize(stmt);

    return true;

}