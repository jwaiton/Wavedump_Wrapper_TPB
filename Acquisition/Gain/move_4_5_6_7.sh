#!/bin/bash

#---- Script to move gain files to storage location

PARENT=18mar22

DESTINATION=/home/user1/Watchman/Wavedump_Wrapper/Storage/${PARENT}

RUN=000003

mv -r /home/user1/Watchman/${PARENT}/RUN${RUN}/ ${DESTINATION}
