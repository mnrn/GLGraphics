# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.hello.Debug:
/Users/n/MyCode/opengl/yryr/bin/Debug/hello:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/Debug/hello


PostBuild.particles.Debug:
/Users/n/MyCode/opengl/yryr/bin/Debug/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/Debug/particles


PostBuild.hello.Release:
/Users/n/MyCode/opengl/yryr/bin/Release/hello:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/Release/hello


PostBuild.particles.Release:
/Users/n/MyCode/opengl/yryr/bin/Release/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/Release/particles


PostBuild.hello.MinSizeRel:
/Users/n/MyCode/opengl/yryr/bin/MinSizeRel/hello:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/MinSizeRel/hello


PostBuild.particles.MinSizeRel:
/Users/n/MyCode/opengl/yryr/bin/MinSizeRel/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/MinSizeRel/particles


PostBuild.hello.RelWithDebInfo:
/Users/n/MyCode/opengl/yryr/bin/RelWithDebInfo/hello:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/RelWithDebInfo/hello


PostBuild.particles.RelWithDebInfo:
/Users/n/MyCode/opengl/yryr/bin/RelWithDebInfo/particles:\
	/usr/local/lib/libGLEW.dylib\
	/usr/local/lib/libglfw.dylib\
	/usr/local/lib/libboost_chrono-mt.a\
	/usr/local/lib/libboost_program_options-mt.a\
	/usr/local/lib/libboost_system-mt.a\
	/usr/local/lib/libboost_filesystem-mt.a\
	/usr/local/lib/libboost_thread-mt.a\
	/usr/local/lib/libboost_date_time-mt.a\
	/usr/local/lib/libboost_regex-mt.a\
	/usr/local/lib/libboost_serialization-mt.a\
	/usr/local/lib/libboost_atomic-mt.a
	/bin/rm -f /Users/n/MyCode/opengl/yryr/bin/RelWithDebInfo/particles




# For each target create a dummy ruleso the target does not have to exist
/usr/local/lib/libGLEW.dylib:
/usr/local/lib/libboost_atomic-mt.a:
/usr/local/lib/libboost_chrono-mt.a:
/usr/local/lib/libboost_date_time-mt.a:
/usr/local/lib/libboost_filesystem-mt.a:
/usr/local/lib/libboost_program_options-mt.a:
/usr/local/lib/libboost_regex-mt.a:
/usr/local/lib/libboost_serialization-mt.a:
/usr/local/lib/libboost_system-mt.a:
/usr/local/lib/libboost_thread-mt.a:
/usr/local/lib/libglfw.dylib:
