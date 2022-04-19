#include "pandaFramework.h"
#include "pandaSystem.h"

#include "genericAsyncTask.h"
#include "asyncTaskManager.h"

PT(AsyncTaskManager) taskMgr = AsyncTaskManager::get_global_ptr();
PT(ClockObject) globalClock = ClockObject::get_global_clock();
NodePath camera;


AsyncTask::DoneStatus spinCameraTask(GenericAsyncTask *task, void *data) {

	double time = globalClock->get_real_time();
	double angledegrees = time * 6.0;
	double angleradians = angledegrees * (3.14 / 180.0);

	camera.set_pos(20 * sin(angleradians), -20.0 * cos(angleradians), 3);
	camera.set_hpr(angledegrees, 0, 0);

	return AsyncTask::DS_cont;
}


int main(int argc, char *argv[]) {
  // Open a new window framework
  PandaFramework framework;
  framework.open_framework(argc, argv);

  // Set the window title and open the window
  framework.set_window_title("My Panda3D Window");
  WindowFramework *window = framework.open_window();

  // Store Camera as a variable
  camera = window->get_camera_group();

  // Load the environment model.
  NodePath scene = window->load_model(framework.get_models(), "models/environment");
  // Reparent the model to render.
  scene.reparent_to(window->get_render());
  // Apply scale and position transforms to the model.
  scene.set_scale(0.25f, 0.25f, 0.25f);
  scene.set_pos(-8, 42, 0);

  // Loads the Panda Model
  NodePath pandaActor = window->load_model(framework.get_models(), "models/panda-model");
  pandaActor.set_scale(0.005);
  pandaActor.reparent_to(window->get_render());

  // Loads the Panda Animation
  window->load_model(pandaActor, "models/panda-walk4");
  window->loop_animations(0);


  // Adds (really bad) camera controls with the mouse.
  // window->setup_trackball();

  // Adds defined camera task from above.
  taskMgr->add(new GenericAsyncTask("Spins the camera", &spinCameraTask, nullptr));


  // Calls Main loop.
  framework.main_loop();

  // Closes framework after window is closed.
  framework.close_framework();
  return (0);
}