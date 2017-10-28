#ifndef CONTROL_INTERFACE_H_
#define CONTROL_INTERFACE_H_

#include <stdbool.h>

struct control_callbacks {
	int	(*presetChanged)(int);
	int	(*bankChanged)(int);
	int	(*modeChanged)(bool);
	int	(*bypassEnabled)(bool);
	int	(*muteEnabled)(bool);
};

int register_callbacks(struct control_callbacks *callbacks);

int set_preset(int preset);
int set_bank(int bank);
int set_mode(bool pedalMode);
int set_bypass(bool enabled);
int set_mute(bool enabled);

int get_preset(int *preset);
int get_bank(int *bank);
int get_mode(bool *pedalMode);
int get_bypass(bool *enabled);
int get_mute(bool *enabled);

#endif // CONTROL_INTERFACE_H_
