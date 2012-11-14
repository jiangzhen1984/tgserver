
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./../:.
export LD_LIBRARY_PATH
TGSERVER_HOME=`pwd`/../

if [ ! -d ../log ]; then
	mkdir ../log
fi

./tgserver $@ >../log/tg.log &

