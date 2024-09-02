# The Dockerfile is based on the official Ubuntu image for the jammy release. 
# The CONNEXT_VERSION argument is used to specify the version of RTI Connext to install. 
# The RTI_LICENSE_AGREEMENT_ACCEPTED argument is used to accept the RTI license agreement during installation.
# The NDDSHOME environment variable is set to the installation directory of RTI Connext.
#
# The Dockerfile installs the RTI Connext Debian Package from the official RTI repository. It also installs some build tools and the license file.
#
# The CMD instruction specifies the default command to run when the container starts. In this case, it runs the /bin/bash shell.
#
# To build the Docker image, run the following command from the root repository folder:
#   docker build -t connext:7.3.0 -f docker/Dockerfile --build-arg RTI_LICENSE_AGREEMENT_ACCEPTED=accepted --build-arg CONNEXT_VERSION=7.3.0 .
#
# To run the Docker container, run the following command:
#   docker run -it --rm connext:7.3.0
#
#   The -it option is used to run the container in interactive mode.
#   The --rm option is used to remove the container when it exits.
#
# The container will start and run the /bin/bash shell.

FROM ubuntu:jammy AS install-stage

ARG CONNEXT_VERSION=7.3.0
ARG RTI_LICENSE_AGREEMENT_ACCEPTED

ENV DISPLAY=:0
ENV SHELL=/bin/bash
ENV TZ=Europe/Madrid
ENV NDDSHOME=/opt/rti.com/rti_connext_dds-${CONNEXT_VERSION}

# Install the required packages
RUN export DEBIAN_FRONTEND=noninteractive \
    && apt-get update \
    && apt-get install -y \
    ca-certificates \
    dash \
    tzdata \
    build-essential \
    cmake \
    libgtk2.0-0 \
    libxxf86vm1 \
    libsm6 \
    libcanberra-gtk-module \
    iproute2 \
    iputils-ping \
    curl \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists

# Install the RTI Connext Debian Package
RUN curl -sSL -o /usr/share/keyrings/rti-official-archive.gpg \
    https://packages.rti.com/deb/official/repo.key

RUN printf -- "deb [arch=%s, signed-by=%s] %s %s main\n" \
    $(dpkg --print-architecture) \
    /usr/share/keyrings/rti-official-archive.gpg \
    https://packages.rti.com/deb/official \
    $(. /etc/os-release && echo ${VERSION_CODENAME}) | tee /etc/apt/sources.list.d/rti-official.list >/dev/null

RUN export DEBIAN_FRONTEND=noninteractive \
    RTI_LICENSE_AGREEMENT_ACCEPTED=${RTI_LICENSE_AGREEMENT_ACCEPTED} \
    && apt-get update \
    && apt-get install -y \
    rti-connext-dds-${CONNEXT_VERSION} \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists

WORKDIR /root

# Copy the source code
COPY CMakeLists.txt /root/
COPY resources /root/resources
COPY src /root/src

# Build the source code
RUN mkdir build && cd build && cmake .. && cmake --build . \
    && rm -rf src /root/resources /root/CMakeLists.txt \
    && mv /root/build /root/app
 
RUN echo "source ${NDDSHOME}/resource/scripts/rtisetenv_*.bash" >> /root/.bashrc

FROM scratch AS final-stage

WORKDIR /root

COPY --from=install-stage / /

CMD ["/bin/bash"]
