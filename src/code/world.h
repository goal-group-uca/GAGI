/**
* @file world.h
* @version 1.0
* @date 25/04/2023
* @author Abdon Crespo Alvarez
* @title World.
* @brief Contains the declaration of the world class, used to configure various
* global aspects of the currently loaded level as well as some additional
* related types.
*/
#ifndef _VOXELENG_VOXELENGWORLD_
#define _VOXELENG_VOXELENGWORLD_
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include "definitions.h"
#include "graphics.h"

#if GRAPHICS_API == OPENGL

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#endif


namespace VoxelEng {

	/////////////////////
	//Type definitions.//
	/////////////////////

	/**
	* @brief The plain colour that the sky that boxes the entire level.
	*/
	typedef color skybox;


	////////////
	//Classes.//
	////////////

	/**
	* @brief Worlds are the greatest level of terrain organization in the engine and are a synonym of level. This class
	* provides capalities that apply globally to an entire world in this engine.
	*/
	class world {
	
	public:

		// Initialisers.

		/**
		* @brief Initialise the class's static members and allocated any resources
		* that are required on initialisation.
		*/
		static void init();


		// Modifiers.

		/**
		* @brief Set the level's skybox.
		*/
		static void setSkybox(const skybox& skybox);

		/**
		* @brief Add a global tick function to the level.
		*/
		static void addGlobalTickFunction(const std::string& name, tickFunc func, bool active = true);

		/**
		* @brief Change the active state of a global tick function registered in the level.
		*/
		static void changeStateGlobalTickFunction(const std::string& name);

		/**
		* @brief Change the active state of a global tick function registered in the level.
		*/
		static void changeStateGlobalTickFunction(const std::string& name, bool active);

		/**
		* @brief Delete a global tick function registered in the level.
		*/
		static void deleteGlobalTickFunction(const std::string& name);

		/**
		* @brief Method to provide the capability of processing the global and entity ticks of the world
		* to a worker thread that is not created inside this method.
		* Effectively will call world::processGlobalTickFunctions() and the entity tick functions counterpart of
		* this method.
		*/
		static void processWorldTicks();

		/**
		* @brief Method to provide the capability of processing the global ticks of the world
		* to a worker thread that is not created inside this method.
		*/
		static void processGlobalTickFunctions();


		// Clean up.

		/**
		* @brief Deinitialise the class's static members and free any resources allocated by them.
		*/
		static void cleanUp();

	private:

		static bool initialised_;
		static std::unordered_map<std::string, tickFunc> globalTickFunctions_;
		static std::unordered_set<std::string> activeTickFunctions_;
		static std::mutex tickFunctionsMutex_;
	
	};

	inline void world::setSkybox(const skybox& background) {

		#if GRAPHICS_API == OPENGL

			glClearColor(background.red() / 255.0f, background.green() / 255.0f, background.blue() / 255.0f, background.alpha());

		#else

			

		#endif
	
	}

}

#endif