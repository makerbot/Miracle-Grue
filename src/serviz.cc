#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "mongoose/mongoose.h"

using namespace std;




void serveFile( const char *filename, struct mg_connection *conn)
{
    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: content-type text/html\r\n\r\n"
              );

    ifstream file;
	file.open (filename);
	if (file.is_open())
	{
		string line;
		while ( file.good() )
		{
			getline (file,line);
			mg_printf(conn, "%s\n",line.c_str());
			cout << line << endl;
		}
		file.close();
	}
	file.close();

}

static void *callback(enum mg_event event,
                      struct mg_connection *conn,
                      const struct mg_request_info *request_info) {
	if (event == MG_NEW_REQUEST)
	{
		string s(request_info->uri);
		if(s=="/favicon.ico")
		{
			return (void*)""; // processed
		}

		if(s=="/")
		{
			serveFile("src/mongoose/app.html", conn);
			return (void*)""; // processed
		}

		if(s=="/stream")
		{
			cout << "#STREAM#" << endl;
			// Echo requested URI back to the client
			mg_printf(conn, "HTTP/1.1 200 OK\r\n"
						  "Content-Type: text/event-stream\r\nCache-Control: no-cache\r\n\r\n");

			for (unsigned int i=0; i < 3; i++)
			{
				mg_printf(conn, "event: update\n");
				mg_printf(conn, "data: {\"msg\":\"hello %d\"}\n\n", i);
			}
			return (void*)"";  // Mark as processed

		}
		cout << "get " << s << endl;




		return (void*)"";  // Mark as processed
	}
	else
	{
		return NULL;
	}
}

int main(void) {
  struct mg_context *ctx;
  const char *options[] = {"listening_ports", "8080", NULL};

  ctx = mg_start(&callback, NULL, options);
  getchar();  // Wait until user hits "enter"
  mg_stop(ctx);

  return 0;
}
