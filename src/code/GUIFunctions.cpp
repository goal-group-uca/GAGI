#include <string>
#include <filesystem>
#include "chunk.h"
#include "gui.h"
#include "graphics.h"
#include "GUIfunctions.h"
#include "logger.h"


namespace VoxelEng {

	namespace GUIfunctions {

		void changeStateMainMenu() {

			GUImanager::changeGUIState("mainMenu");
			if (game::selectedEngineMode() == VoxelEng::engineMode::EDITLEVEL) {

				GUImanager::changeGUIState("mainMenu.saveButton");
				GUImanager::changeGUIState("mainMenu.loadButton");

			}
			else {

				GUImanager::changeGUIState("mainMenu.newButton");
				GUImanager::changeGUIState("mainMenu.loadButton");

			}
			GUImanager::changeGUIState("mainMenu.exitButton");

		}

		void changeStateLevelMenu() {

			if (game::selectedEngineMode() == VoxelEng::engineMode::EDITLEVEL) {
			
				GUImanager::switchLevelGUIOpened();
				changeStateMainMenu();
				graphics::getMainWindow()->changeStateMouseLock();
			
			}

		}

		void changeStateMainMenu(bool isEnabled) {
		
			GUImanager::changeGUIState("mainMenu", isEnabled);
			GUImanager::changeGUIState("mainMenu.loadButton", isEnabled);
			if (game::selectedEngineMode() == VoxelEng::engineMode::EDITLEVEL) {
			
				GUImanager::changeGUIState("mainMenu.saveButton", isEnabled);
				GUImanager::changeGUIState("mainMenu.loadButton", isEnabled);
			
			}
			else {

				GUImanager::changeGUIState("mainMenu.newButton", isEnabled);
				GUImanager::changeGUIState("mainMenu.loadButton", isEnabled);

			}
			GUImanager::changeGUIState("mainMenu.exitButton", isEnabled);
		
		}

		void showLoadMenu() {

			game::setSlotAccessType(slotAccessType::load);

			changeStateMainMenu(false);
			GUImanager::changeGUIState("loadMenu", true);
			GUImanager::changeGUIState("loadMenu.exitButton", true);

			for (int i = 1; i <= 5; i++)
				GUImanager::changeGUIState("saveSlot" + std::to_string(i), true);

		}

		void hideLoadMenu() {

			GUImanager::changeGUIState("loadMenu", false);
			GUImanager::changeGUIState("loadMenu.exitButton", false);

			for (int i = 1; i <= 5; i++)
				GUImanager::changeGUIState("saveSlot" + std::to_string(i), false);

			changeStateMainMenu(true);

		}

		void showSaveMenu() {

			game::setSlotAccessType(slotAccessType::save);

			changeStateMainMenu(false);
			GUImanager::changeGUIState("saveMenu", true);
			GUImanager::changeGUIState("saveMenu.exitButton", true);

			for (int i = 1; i <= 5; i++)
				GUImanager::changeGUIState("saveSlot" + std::to_string(i), true);

		}

		void hideSaveMenu() {

			GUImanager::changeGUIState("saveMenu", false);
			GUImanager::changeGUIState("saveMenu.exitButton", false);

			for (int i = 1; i <= 5; i++)
				GUImanager::changeGUIState("saveSlot" + std::to_string(i), false);

			changeStateMainMenu(true);

		}

		void saveGame() {

			// Convert character to int number (assuming the last character of the GUIElement's name is a digit).
			unsigned int saveSlot = GUImanager::lastCheckedGUIElement()->name().back() - '0';
			game::setSaveSlot(saveSlot);

			// Save chunk data into selected save slot.
			chunkManager::saveAllChunks("saves/slot" + std::to_string(saveSlot) + "/level");

			logger::debugLog("Saved on slot " + std::to_string(saveSlot));


		}

		void loadGame() {

			GUImanager::setLevelGUIOpened(false);

			// Convert character to int number (assuming the last character of the GUIElement's name is a digit).
			unsigned int saveSlot = GUImanager::lastCheckedGUIElement()->name().back() - '0';
			game::setSaveSlot(saveSlot);

			// Check if save slot has valid data written.
			if (std::filesystem::exists(std::filesystem::path("saves/slot" + std::to_string(game::selectedSaveSlot()) + "/level.terrain"))) {

				// Hide load menu and don't show the main menu like the hideLoadMenu() function.
				GUImanager::changeGUIState("loadMenu", false);
				GUImanager::changeGUIState("loadMenu.exitButton", false);

				for (int i = 1; i <= 5; i++)
					GUImanager::changeGUIState("saveSlot" + std::to_string(i), false);

				game::setLoopSelection(VoxelEng::engineMode::INITLEVEL);

				logger::debugLog("Selected to load slot " + std::to_string(saveSlot));
			
			}
			else
				logger::debugLog("The selected slot is empty");

		}

		void enterNewLevel() {
		
			GUImanager::setLevelGUIOpened(false);

			changeStateMainMenu(false);

			game::setLoopSelection(VoxelEng::engineMode::INITLEVEL);
			game::setSaveSlot(0);
			
		}

		void accessSaveSlot() {
		
			if (game::getSlotAccessType() == slotAccessType::load)
				loadGame();
			else {
			
				hideSaveMenu();

				saveGame();
			
			}
			
		}

		void exit() {
		
			switch (game::selectedEngineMode()) {
			
				case VoxelEng::engineMode::AIMENULOOP:

					game::setLoopSelection(VoxelEng::engineMode::EXIT);

					break;

				case VoxelEng::engineMode::GRAPHICALMENU:

					game::setLoopSelection(VoxelEng::engineMode::AIMENULOOP);

					break;

				case VoxelEng::engineMode::EDITLEVEL:

					GUImanager::changeGUIState("mainMenu.saveButton");
					GUImanager::changeGUIState("mainMenu.newButton");

					game::setLoopSelection(VoxelEng::engineMode::EXITLEVEL);
					game::setLoopSelection(VoxelEng::engineMode::GRAPHICALMENU);

					break;

				case VoxelEng::engineMode::PLAYINGRECORD:

					game::setLoopSelection(VoxelEng::engineMode::EXITRECORD);
					game::setLoopSelection(VoxelEng::engineMode::AIMENULOOP);

					break;
			
			}
		
		}

	}

}