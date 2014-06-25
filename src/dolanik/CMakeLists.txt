list(APPEND dolan_SRC src/dolanik/dolanik.cpp
		      src/dolanik/music.cpp
		      src/dolanik/server.cpp
		      src/dolanik/filePlayer.cpp
		      src/dolanik/fileSong.cpp
		      src/dolanik/resampler.cpp
		      src/dolanik/state.cpp
)

set(dolan_SRC "${dolan_SRC}" PARENT_SCOPE)