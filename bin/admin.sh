
function help() {
cat <<EOF
#########################################################
 TGServer administration terminal   
     0: shutdown server 
     1: display all connected clients   
     2: shutdown client according to pid 
     3: send message by input    
     q: quit 
#########################################################
EOF

}

function tgcat() {
	echo " pid    ip       port"
	while read -t 2 ci </tmp/tgo
	do
		echo $ci
	done
}
	

function tgconn() {

	local answer 
	while [ true ]
	do
		help
		echo -n " please choose command:"	
		read answer
		if [ "$answer" == "0" ]; then
			echo -n "tg://1003/" > /tmp/tgp
		elif [ "$answer" == "1" ]; then
			echo -n "tg://1040/" > /tmp/tgp
			tgcat
		elif [ "$answer" == "2" ]; then
			local id
			echo -n " please input client pid:"
			read id
			echo -n "tg://1041/$id" > /tmp/tgp	
		elif [ "$answer" == "q" ]; then
			break	
		else
			echo " unkown command"
		fi
	done
}


tgconn

