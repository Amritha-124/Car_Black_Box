#ifndef CAR_BLACK_BOX_H
#define CAR_BLACK_BOX_H
void display_dashboard(unsigned char key);
unsigned char password_authentication(unsigned char sw);
void display_menu(unsigned char sw);
void store_log(unsigned char*);
void view_log(unsigned char sw);
void clear_log(unsigned char sw);
void download_log(unsigned char sw);
void change_password(unsigned char sw);
void set_time(unsigned char sw);
#endif