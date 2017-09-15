#ifndef REMOTE_CONTROL_H_
#define REMOTE_CONTROL_H_

#include "button_control.h"

struct remote_control {
	int		(*callback)(void *, enum ae_button, enum button_state);
	void	*context;
};

#define REMOTE_CONTROL_INITIALIZER() { 0 }

int remote_control_init(struct button_control *remote, int (*callback)(void *, enum ae_button), void *context);

int remote_control_uninit(struct button_control *remote);

#endif // REMOTE_CONTROL_H_
