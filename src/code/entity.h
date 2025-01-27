/**
* @file entity.h
* @version 1.0
* @date 20/04/2023
* @author Abdon Crespo Alvarez
* @title Entity.
* @brief Contains the player, entity and entityManager classes as well as some
* auxiliary data structures and types used with them.
*/
#ifndef _VOXENG_ENTITY_
#define _VOXENG_ENTITY_
#include <atomic>
#include <condition_variable>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

#if GRAPHICS_API == OPENGL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#endif

#include "batch.h"
#include "camera.h"
#include "chunk.h"
#include "definitions.h"
#include "gameWindow.h"
#include "model.h"


namespace VoxelEng {

	/////////////////////////
	//Forward declarations.//
	/////////////////////////

	class batch;
	class entityManager;


	////////////
	//Classes.//
	////////////

	/**
	* @brief Abstraction containing everything that defines the entity that
	* represents the user inside the engine.
	*/
	class player {

	public:

		// Initialisation.

		/**
		* @brief Initialise the player system and all the class' static attributes.
		*/
		static void init(float FOV, float zNear, float zFar, window& window, unsigned int blockReachRange);


		// Modifiers.

		/**
		* @brief Get the user's first person camera.
		*/
		static camera& getCamera();

		/**
		* @brief Computes a basic raycast to select a block in the world that is
		* reachable from where the user stands.
		*/
		static void selectBlock();

		/**
		* @brief Callback function to get the user's mouse buttons input.
		*/
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

		/**
		* @brief Destroys the block selected by the user's camera, if any.
		*/
		static void destroySelectedBlock();

		/**
		* @brief Places the current selected block where the user is looking at.
		* Cannot place a block on thin air, must be looking into a solid block to place.
		*/
		static void placeSelectedBlock();

		/**
		* @brief Function to process user's raycast used to interact with the level's enviroment.
		* WARNING. Must not be called in the rendering thread.
		*/
		static void processSelectionRaycast();

		/**
		* @brief Change the current block ID used by the player to place blocks.
		*/
		static void setBlockToPlace(block blockID);

		/**
		* @brief Change user position and viewing direction.
		*/
		static void changePosition(const vec3& newPos, const vec3& newDir = vec3Zero);

		/**
		* @brief Change user position and viewing direction.
		*/
		static void changePosition(int newX, int newY, int newZ, int newDirX = 0, int newDirY = 0, int newDirZ = 0);


		// Clean up.

		/**
		* @brief Used to clean up the heap memory allocated by this system and deinitialise it.
		*/
		static void cleanUp();

	private:

		static bool initialised_;
		static GLFWwindow* window_;
		static camera* camera_;
		static float blockReachRange_,
			         blockSearchIncrement_;
		static block selectedBlock_;
		static std::atomic<block> blockToPlace_;
		static vec3 selectedBlockPos_,
			        oldSelectedBlockPos_;
		

		/*
		Flags used to coordinate the callbacks called
		on the rendering thread with the input processing thread.
		*/
		static std::atomic<bool> destroyBlock_,
					      placeBlock_;

	};

	inline camera& player::getCamera() {

		return *camera_;

	}

	inline void player::changePosition(const vec3& newPos, const vec3& newDir) {
	
		changePosition(newPos.x, newPos.y, newPos.z, newDir.x, newDir.y, newDir.z);
	
	}


	/**
	* @brief An entity possess a custom 3D model as its representation
	* inside a level and is capable of being associated with actions
	* that affect other entities or the level itself.
	*/
	class entity {

	public:

		// Constructors.

		/**
		* @brief Class constructor.
		* WARNING. To create an entity use entityManager::registerEntity(). Otherwise the
		* created entity will not be registered in the entity management system.
		*/
		entity(unsigned int modelID, const vec3& pos, const vec3& rot, tickFunc func = nullptr);


		// Observers.

		/**
		* @brief Get the entity's position.
		*/
		const vec3& pos() const;

		/**
		* @brief Get entity's postion in X axis.
		*/
		float x() const;

		/**
		* @brief Get entity's postion in Y axis.
		*/
		float y() const;

		/**
		* @brief Get entity's postion in Z axis.
		*/
		float z() const;

		/**
		* @brief Get the entity's rotation.
		*/
		const vec3& rot() const;

		/**
		* @brief Get the entity's model.
		*/
		const model& entityModel() const;

		/**
		* @brief Flag to check if model X rotation needs to be updated in the model's vertices.
		*/
		bool updateXRotation() const;

		/**
		* @brief Flag to check if model Y rotation needs to be updated in the model's vertices.
		*/
		bool updateYRotation() const;

		/**
		* @brief Flag to check if model Z rotation needs to be updated in the model's vertices.
		*/
		bool updateZRotation() const;

		/**
		* @brief Get sin(entity's last rotation angle) in X-axis
		*/
		float sinAngleX() const;

		/**
		* @brief Get cos(entity's last rotation angle) in X-axis.
		*/
		float cosAngleX() const;

		/**
		* @brief Get sin(entity's last rotation angle) in Y-axis.
		*/
		float sinAngleY() const;

		/**
		* @brief Get cos(entity's last rotation angle) in Y-axis.
		*/
		float cosAngleY() const;

		/**
		* @brief Get sin(entity's last rotation angle) in Z-axis.
		*/
		float sinAngleZ() const;

		/**
		* @brief Get cos(entity's last rotation angle) in Z-axis.
		*/
		float cosAngleZ() const;


		// Modifiers.

		/**
		* @brief Get the entity's position.
		*/
		vec3& pos();

		/**
		* @brief Set entity's postion in X axis.
		*/ 
		float& x();

		/**
		* @brief Set entity's postion in Y axis.
		*/
		float& y();

		/**
		* @brief Set entity's postion in Z axis.
		*/
		float& z();

		/**
		* @brief Change the entity's model.
		*/
		void setModelID(unsigned int modelID);

		/**
		* @brief Rotate the entity.
		*/
		void rotate(const vec3& rotation);

		/**
		* @brief Rotate the entity.
		*/
		void rotate(float x, float y, float z);

		/**
		* @brief Rotate the entity's model in the X-axis
		*/
		void rotateX(float angle);

		/**
		* @brief Rotate the entity's model in the Y-axis
		*/
		void rotateY(float angle);

		/**
		* @brief Rotate the entity's model in the Z-axis
		*/
		void rotateZ(float angle);

		/**
		* @brief Rotates the entity's view direction, considered as a vector with direction
		* towards the front of the model, and consequently rotates the entity's model so that
		* said view direction vector still points to the model's front.
		* The vec3 direction is considered to contain the roll, pitch and yaw angles in said order,
		* with the roll angle being the first element.
		*/
		void rotateView(const vec3& rotation);

		/**
		* @brief Rotates the entity's view direction, considered as a vector with direction
		* towards the front of the model, and consequently rotates the entity's model so that
		* said view direction vector still points to the model's front.
		*/
		void rotateView(float roll, float pitch, float yaw);

		/**
		* @brief Rotates the entity's view direction (only the roll angle), considered as a vector with direction
		* towards the front of the model, and consequently rotates the entity's model so that
		* said view direction vector still points to the model's front.
		*/
		void rotateViewRoll(float angle);

		/**
		* @brief Rotates the entity's view direction (only the pitch angle), considered as a vector with direction
		* towards the front of the model, and consequently rotates the entity's model so that
		* said view direction vector still points to the model's front.
		*/
		void rotateViewPitch(float angle);

		/**
		* @brief Rotates the entity's view direction (only the yaw angle), considered as a vector with direction
		* towards the front of the model, and consequently rotates the entity's model so that
		* said view direction vector still points to the model's front.
		*/
		void rotateViewYaw(float angle);
		
	private:

		/*
		Friend classes.
		*/

		friend entityManager;


		/*
		Attributes.
		*/

		static const float piDiv;

		bool updateXRotation_,
			 updateYRotation_,
			 updateZRotation_;
		vec3 pos_,
			 rot_;
		const model* model_;
		tickFunc tickFunc_;

	};

	inline const vec3& entity::pos() const {
	
		return pos_;
	
	}

	inline const vec3& entity::rot() const {
	
		return rot_;
	
	}

	inline float entity::x() const {
	
		return pos_.x;
	
	}

	inline float entity::y() const {

		return pos_.y;

	}

	inline float entity::z() const {

		return pos_.z;

	}

	inline const model& entity::entityModel() const {

		return *model_;

	}

	inline bool entity::updateXRotation() const {
	
		return updateXRotation_;
	
	}

	inline bool entity::updateYRotation() const {

		return updateYRotation_;

	}

	inline bool entity::updateZRotation() const {

		return updateZRotation_;

	}

	inline float entity::sinAngleX() const {

		return std::sin(rot_.x * piDiv);
	
	}

	inline float entity::cosAngleX() const {

		return std::cos(rot_.x * piDiv);

	}

	inline float entity::sinAngleY() const {

		return std::sin(rot_.y * piDiv);

	}

	inline float entity::cosAngleY() const {

		return std::cos(rot_.y * piDiv);

	}

	inline float entity::sinAngleZ() const {

		return std::sin(rot_.z * piDiv);

	}

	inline float entity::cosAngleZ() const {

		return std::cos(rot_.z * piDiv);

	}

	inline vec3& entity::pos() {

		return pos_;

	}

	inline float& entity::x() {

		return pos_.x;

	}

	inline float& entity::y() {

		return pos_.y;

	}

	inline float& entity::z() {

		return pos_.z;

	}

	inline void entity::rotate(const vec3& rot) {

		rotate(rot.x, rot.y, rot.z);

	}

	inline void entity::rotateView(const vec3& rotation) {

		rotateView(rotation.x, rotation.y, rotation.z);

	}

	inline void entity::setModelID(unsigned int modelID) {
	
		model_ = &models::getModelAt(modelID);
	
	}


	/**
	* @brief This class is in charge of encapsulating and abstracting
	* all related to the management of entities in the engine, including
	* the entity part of the AI agents.
	*/
	class entityManager {

	public:

		// Initialisation.

		/**
		* @brief Initialise the entity management system.
		* Allocate any resources that are needed on initialisation.
		*/
		static void init();


		// Observers.

		/**
		* @brief Returns true if the entity management system is initialised.
		*/
		static bool initialised();

		/**
		* @brief Returns true if the specified entity ID correponds to a registered one or false otherwise.
		* An entityID that belongs to a deleted (or rather "freed") entity does not count
		* as an registered entity until a new call to any overload of entityManager::registerEntity()
		* reuses said ID (and returns it as the call's result) to represent another entity.
		*/
		static bool isEntityRegistered(entityID entityID);

		/**
		* @brief Returns true if the specified entity is active (with bounds checking).
		*/
		static bool isEntityActiveAt(entityID entityID);

		/**
		* @brief Returns true if the specified entity is active (without bounds checking).
		*/
		static bool isEntityActive(entityID entityID);


		// Modifiers: general.

		/**
		* @brief Sets the entity management system's AI mode on or off.
		* Intended to be use inside the game::setAImode method only.
		*/
		static void setAImode(bool on);

		/**
		* @brief Function used by a worker thread to manage entities in a world/level.
		* Management includes creating, updating and removing entities from the world.
		*/
		static void manageEntities();

		/**
		* @brief Return the total number of entity IDs used (even for freed entity objects).
		*/
		static unsigned int nEntities();

		/**
		* @brief Registers a new entity in the entity management system.
		* Returns the ID of the registered entity.
		*/
		static entityID registerEntity(unsigned int modelID, const vec3& pos, const vec3& rot = vec3Zero, tickFunc func = nullptr);

		/**
		* @brief Registers a new entity in the entity management system.
		* Returns the ID of the registered entity.
		*/
		static entityID registerEntity(unsigned int modelID, int posX, int posY, int posZ, const vec3& rot = vec3Zero, tickFunc func = nullptr);

		/**
		* @brief Registers a new entity in the entity management system.
		* Returns the ID of the registered entity.
		*/
		static entityID registerEntity(unsigned int modelID, int posX, int posY, int posZ, float rotX, float rotY, float rotZ, tickFunc func = nullptr);

		/**
		* @brief Returns the entity with the specified entity ID (with bounds checking).
		*/
		static entity& getEntityAt(entityID ID);

		/**
		* @brief Returns the entity with the specified entity ID (without bounds checking).
		*/
		static entity& getEntity(entityID ID);

		/**
		* @brief Changes the entity's active state (without bounds checking).
		*/
		static void changeEntityActiveStateAt(entityID ID, bool active);

		/**
		* @brief Deletes an existing entity from the entity management system.
		* The entity ID remains free to be reused for other new entities.
		*/
		static void deleteEntityAt(entityID ID);

		/**
		* @brief Deletes an existing entity from the entity management system.
		* The entity ID remains free to be reused for other new entities.
		*/
		static void deleteEntity(entityID ID);

		/**
		* @brief Swaps read-only and write-only batch rendering data.
		* WARNING. Rendering-thread and entity management thread must be synced when calling this function
		*/
		static void swapReadWrite();

		/**
		* @brief Get condition variable used for syncing the rendering and the batching threads.
		*/
		static std::condition_variable& entityManagerCV();

		/**
		* @brief Get condition variable flag used for syncing the rendering and the batching threads.
		*/
		static bool syncFlag();

		/**
		* @brief Get mutex used for syncing the rendering and the batching threads.
		*/
		static std::mutex& syncMutex();

		/**
		* @brief Get the readable rendering data necessary for the rendering thread to render the batches properly.
		*/
		static const std::vector<model>* renderingData();


		// Modifiers: actions on entities.

		static void moveEntity(entityID entityID, int x, int y, int z);

		
		// Clean up.

		/**
		* @brief Free dynamic memory allocated by the entity manager system and NOT deinitialise it.
		*/
		static void clean();

		/**
		* @brief Free dynamic memory allocated by the entity manager system and deinitialise it.
		*/
		static void cleanUp();

	private:

		/*
		Attributes.
		*/

		static bool initialised_,
					firstManagementIteration_;
		static std::vector<entity> entities_;
		static std::vector<batch> batches_;

		static std::unordered_map<unsigned int, unsigned int> entityBatch_; // Relates entity's ID with the batch it belongs to.
		static std::unordered_set<unsigned int> activeEntityID_,
												activeBatchID_,
											    freeEntityID_,
												freeBatchID_,
												inactiveEntityID_,
												inactiveBatchID_,
												deleteableEntityID_;

		// Queue of entities' ID that belong to entities that are
		// waiting for their tick method (with its respective pointer different from nullptr)
		// to execute.
		static std::list<unsigned int> tickingEntityID_; 	

		static std::vector <model>* renderingDataWrite_,
						          * renderingDataRead_;
		
		static std::recursive_mutex entitiesMutex_,
									batchesMutex_;
		static std::condition_variable entityManagerCV_;
		static std::atomic<bool> entityMngCVContinue_;
		static std::mutex syncMutex_;
		static std::unique_lock<std::mutex> syncLock_;
		static unsigned int ticksPerFrame_, // Used to distribute tick function executions between frame.
							opsPerFrame_; // Used to distribute some operations between frames.


		/*
		Methods.
		*/

		// Observers.

		/*
		Note: freed batches do not count as registered as they
		are 'deleted' but they actually are not in order to avoid
		unnecessary allocations.
		*/
		static bool isBatchRegistered_(unsigned int batchID);

		static bool isBatchActiveAt_(unsigned int batchID);

		static bool isBatchActive_(unsigned int batchID);


		// Modifiers.

		/*
		Method called by a batch automatically while it is being constructed to register itself
		in the entityManager's batch list. This allows the manager to access to all the entities in the world.
		Batches are used to save drawing calls, but the game logic for the entities is calculated by the method
		entityManager::manager.
		Returns the ID of the registered batch.
		*/
		static unsigned int registerBatch_();

		static void changeBatchActiveStateAt_(unsigned int batchID, bool active);

		static void changeBatchActiveState_(unsigned int batchID, bool active);

		/*
		Marks a batch as free to be reused later.
		*/
		static void deleteBatchAt_(unsigned int batchID);

		/*
		Marks a batch as free to be reused later.
		*/
		static void deleteBatch_(unsigned int batchID);


	};

	inline bool entityManager::initialised() {
	
		return initialised_;
	
	}

	inline entityID entityManager::registerEntity(unsigned int entityTypeID, const vec3& pos, const vec3& rot, tickFunc func) {
	
		return registerEntity(entityTypeID, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, func);
	
	}

	inline entityID entityManager::registerEntity(unsigned int entityTypeID, int posX, int posY, int posZ, const vec3& rot, tickFunc func) {

		return registerEntity(entityTypeID, posX, posY, posZ, rot.x, rot.y, rot.z, func);

	}

	inline std::condition_variable& entityManager::entityManagerCV() {
	
		return entityManagerCV_;
	
	}

	inline bool entityManager::syncFlag() {
	
		return entityMngCVContinue_;
		
	}

	inline std::mutex& entityManager::syncMutex() {
	
		return syncMutex_;
	
	}

	inline const std::vector<model>* entityManager::renderingData() {
	
		return renderingDataRead_;
	
	}

}

#endif