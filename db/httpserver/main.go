package main

import (
	"database/sql"
	"fmt"
	"github.com/gorilla/mux"
	_ "github.com/lib/pq"
	"log"
	"net/http"
)

var db *sql.DB

func initDB() {
	var err error
	connStr := "user=n.gureev dbname=postgres password=rekareka host=185.185.69.197 sslmode=disable"
	db, err = sql.Open("postgres", connStr)
	if err != nil {
		log.Fatalf("Error opening database: %q", err)
	}
}

func getTerminalMasterKey(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	terminalUUID := vars["terminal-uuid"]

	var masterKey string
	err := db.QueryRow(`
		SELECT master_key FROM terminal_master_key
		WHERE terminal_id = $1`,
		terminalUUID).Scan(&masterKey)

	if err != nil {
		if err == sql.ErrNoRows {
			http.Error(w, "Not Found", http.StatusNotFound)
		} else {
			http.Error(w, err.Error(), http.StatusInternalServerError)
		}
		return
	}
	w.Write([]byte(masterKey))
}

func main() {
	initDB()
	defer db.Close()

	r := mux.NewRouter()
	r.HandleFunc("/api/v1/terminal-master-key/{terminal-uuid}", getTerminalMasterKey).Methods("GET")

	http.Handle("/", r)

	port := "9998"
	fmt.Printf("Server is running on port %s\n", port)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}
