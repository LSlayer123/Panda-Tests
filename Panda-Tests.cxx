#include "pandaFramework.h"
#include "pandaSystem.h"

#include "genericAsyncTask.h"
#include "asyncTaskManager.h"

#include "cIntervalManager.h"
#include "cLerpNodePathInterval.h"
#include "cMetaInterval.h"

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

  // Definining intervals for the Panda to walk through in the space.
  PT(CLerpNodePathInterval) pandaPosInterval1, pandaPosInterval2, pandaHprInterval1, pandaHprInterval2;

  // Setting each interval's values separately (Maybe there's a better way of doing this? - i.e. separate function or something)
  pandaPosInterval1 = new CLerpNodePathInterval("pandaPosInterval1", 13.0, CLerpInterval::BT_no_blend, true, false, pandaActor, NodePath());
  pandaPosInterval1->set_start_pos(LPoint3(0, 10, 0));
  pandaPosInterval1->set_end_pos(LPoint3(0, -10, 0));

  pandaPosInterval2 = new CLerpNodePathInterval("pandaPosInterval2", 13.0, CLerpInterval::BT_no_blend, true, false, pandaActor, NodePath());
  pandaPosInterval2->set_start_pos(LPoint3(0, 10, 0));
  pandaPosInterval2->set_end_pos(LPoint3(0, -10, 0));

  pandaHprInterval1 = new CLerpNodePathInterval("pandaHprInterval1", 3.0, CLerpInterval::BT_no_blend, true, false, pandaActor, NodePath());
  pandaHprInterval1->set_start_hpr(LPoint3(0, 0, 0));
  pandaHprInterval1->set_end_hpr(LPoint3(180, 0, 0));

  pandaHprInterval2 = new CLerpNodePathInterval("pandaHprInterval2", 3.0, CLerpInterval::BT_no_blend, true, false, pandaActor, NodePath());
  pandaHprInterval2->set_start_hpr(LPoint3(180, 0, 0));
  pandaHprInterval2->set_end_hpr(LPoint3(0, 0, 0));

  // Creating the sequence that coordinates the intervals
  PT(CMetaInterval) pandaPace;
  pandaPace = new CMetaInterval("pandaPace");

  pandaPace->add_c_interval(pandaPosInterval1, 0, CMetaInterval::RS_previous_end);
  pandaPace->add_c_interval(pandaPosInterval2, 0, CMetaInterval::RS_previous_end);
  pandaPace->add_c_interval(pandaHprInterval1, 0, CMetaInterval::RS_previous_end);
  pandaPace->add_c_interval(pandaHprInterval2, 0, CMetaInterval::RS_previous_end);

  // Looping the sequence
  pandaPace->loop();


  // Adds defined camera task from above.
  taskMgr->add(new GenericAsyncTask("Spins the camera", &spinCameraTask, nullptr));


  // Alteranate way of performing a task every frame without tasks
  Thread* current_thread = Thread::get_current_thread();
  while (framework.do_frame(current_thread)) {
	  CIntervalManager::get_global_ptr()->step();
  }


  // Calls Main loop.
  framework.main_loop();

  // Closes framework after window is closed.
  framework.close_framework();
  return (0);
}