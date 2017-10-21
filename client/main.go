package main

import (
	"context"
	"flag"
	"io"
	"log"
	"net"
	"net/http"
	"os"
)

func main() {
	var fpath string
	flag.StringVar(&fpath, "f", "", "unix domain socket path")
	flag.Parse()

	c := http.Client{
		Transport: &http.Transport{
			DialContext: func(_ context.Context, _, _ string) (net.Conn, error) {
				return net.Dial("unix", fpath)
			},
		},
	}

	resp, err := c.Get("http://unix/")
	if err != nil {
		log.Fatal(err)
	}
	defer resp.Body.Close()
	if _, err := io.Copy(os.Stdout, resp.Body); err != nil {
		log.Fatal(err)
	}
}
