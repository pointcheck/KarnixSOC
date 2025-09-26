digraph USBReceiver {

	//labelloc="t";
	//label="USB 1.0 Controller USBSendData FSM";

	rankdir=TB; // Layout from top to bottom
	//graph [splines = ortho];
	//layout=twopi

	define(`digraph',`subgraph')
	include(`USBReceiver1.dot')
	include(`USBReceiver2.dot')
	include(`USBReceiver3.dot')
}

