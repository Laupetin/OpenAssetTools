FROM ubuntu:focal

RUN /bin/sh -c "dpkg --add-architecture i386"
RUN /bin/sh -c "apt-get update && apt-get upgrade -y && apt-get install -y git make g++ gcc-9 g++-9 gcc-9-multilib g++-9-multilib"

CMD ["/bin/bash"]