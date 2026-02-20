#include "VEInclude.h"
#include "irrKlang.h"
#include "MyEventListener.h"
#include "ECS.h"

namespace ve {


ECS g_ecs;
EntityID g_player = 0;
EntityID g_rifle = 0;
EntityID g_potion = 0;



	///user defined manager class, derived from VEEngine
	class MyVulkanEngine : public VEEngine {
	public:
		MyVulkanEngine(veRendererType type = veRendererType::VE_RENDERER_TYPE_FORWARD, bool debug = false) : VEEngine(type, debug) {};
		~MyVulkanEngine() {};

		irrklang::ISoundEngine* m_irrklangEngine = irrklang::createIrrKlangDevice();

		///Register an event listener to interact with the user
		virtual void registerEventListeners();

		void playSound2D(const std::string& path) {
			if (m_irrklangEngine)
				m_irrklangEngine->play2D(path.c_str(), false);
		}

		EntityID createWallFromMinMax(
			VESceneManager* sm,
			VESceneNode* parent,
			glm::vec3 min,
			glm::vec3 max,
			float height,   
			float thickness,
			const std::string& name
		) {
	
			glm::vec3 center = (min + max) * 0.5f;
			
			glm::vec3 size;
			size.x = fabs(max.x - min.x);
			size.z = fabs(max.z - min.z);
			
			if (size.x < 0.01f) size.x = thickness;
			if (size.z < 0.01f) size.z = thickness;

			size.y = height; 

			VESceneNode* wall;
			VECHECKPOINTER(wall = sm->loadModel(
				name,
				"../../media/models/test/crate0",
				"cube.obj",
				0,
				parent
			));

			wall->setTransform(
				glm::translate(glm::mat4(1.0f), center) *
				glm::scale(glm::mat4(1.0f), size)
			);

			EntityID e = g_ecs.createEntity();
			g_ecs.addComponent<RenderComponent>(e)->node = wall;
			g_ecs.addComponent<HitboxComponent>(e)->size = size;

			g_levelEntities.push_back(e);
			return e;
		}



		void buildLevel(VESceneManager* sm, VESceneNode* root) {

    		createWallFromMinMax(sm, root, {-2,1,-2}, {-2,1,50}, 2.0f, 1.0f, "Wall_1_1");
    		createWallFromMinMax(sm, root, {-2,1,-2}, {50,1,-2}, 2.0f, 1.0f, "Wall_1_2");
			createWallFromMinMax(sm, root, {-2,1,50}, {30,1,50}, 2.0f, 1.0f, "Wall_1_3");
    		createWallFromMinMax(sm, root, {50,1,-2}, {50,1,50}, 2.0f, 1.0f, "Wall_1_4");
			createWallFromMinMax(sm, root, {50,1,50}, {40,1,50}, 2.0f, 1.0f, "Wall_1_5");

    		createWallFromMinMax(sm, root, {-2,1,50}, {-2,1,100}, 2.0f, 1.0f, "Wall_2_1");
			createWallFromMinMax(sm, root, {-2,1,100}, {50,1,100}, 2.0f, 1.0f, "Wall_2_2");
			createWallFromMinMax(sm, root, {50,1,100}, {50,1,80}, 2.0f, 1.0f, "Wall_2_3");
			createWallFromMinMax(sm, root, {50,1,50}, {50,1,70}, 2.0f, 1.0f, "Wall_2_4");

			createWallFromMinMax(sm, root, {50,1,80}, {70,1,80}, 2.0f, 1.0f, "Wall_2_21");
			createWallFromMinMax(sm, root, {50,1,70}, {70,1,70}, 2.0f, 1.0f, "Wall_2_22");

			createWallFromMinMax(sm, root, {70,1,70}, {70,1,50}, 2.0f, 1.0f, "Wall_3_1");
			createWallFromMinMax(sm, root, {70,1,50}, {100,1,50}, 2.0f, 1.0f, "Wall_3_2");
			createWallFromMinMax(sm, root, {100,1,50}, {100,1,60}, 2.0f, 1.0f, "Wall_3_3");
			createWallFromMinMax(sm, root, {100,1,60}, {85,1,60}, 2.0f, 1.0f, "Wall_3_4");
			createWallFromMinMax(sm, root, {85,1,60}, {85,1,120}, 2.0f, 1.0f, "Wall_3_5");
			createWallFromMinMax(sm, root, {70,1,80}, {70,1,130}, 2.0f, 1.0f, "Wall_3_6");

			createWallFromMinMax(sm, root, {70,1,130}, {140,1,130}, 2.0f, 1.0f, "Wall_4_1");
			createWallFromMinMax(sm, root, {140,1,130}, {140,1,120}, 2.0f, 1.0f, "Wall_4_2");
			createWallFromMinMax(sm, root, {85,1,120}, {120,1,120}, 2.0f, 1.0f, "Wall_4_3");
			createWallFromMinMax(sm, root, {70,1,80}, {70,1,130}, 2.0f, 1.0f, "Wall_4_4");

			createWallFromMinMax(sm, root, {140,1,120}, {170,1,120}, 2.0f, 1.0f, "Wall_5_1");
			createWallFromMinMax(sm, root, {170,1,120}, {170,1,90}, 2.0f, 1.0f, "Wall_5_2");
			createWallFromMinMax(sm, root, {170,1,90}, {160,1,90}, 2.0f, 1.0f, "Wall_5_3");
			createWallFromMinMax(sm, root, {150,1,90}, {120,1,90}, 2.0f, 1.0f, "Wall_5_4");
			createWallFromMinMax(sm, root, {120,1,90}, {120,1,120}, 2.0f, 1.0f, "Wall_5_5");

			createWallFromMinMax(sm, root, {150,1,90}, {150,1,30}, 2.0f, 1.0f, "Wall_6_1");
			createWallFromMinMax(sm, root, {150,1,30}, {190,1,30}, 2.0f, 1.0f, "Wall_6_2");
			createWallFromMinMax(sm, root, {160,1,90}, {160,1,50}, 2.0f, 1.0f, "Wall_6_3");
			createWallFromMinMax(sm, root, {160,1,50}, {180,1,50}, 2.0f, 1.0f, "Wall_6_4");

			createWallFromMinMax(sm, root, {180,1,50}, {180,1,140}, 2.0f, 1.0f, "Wall_7_1");
			createWallFromMinMax(sm, root, {190,1,30}, {190,1,170}, 2.0f, 1.0f, "Wall_7_2");
			createWallFromMinMax(sm, root, {190,1,170}, {120,1,170}, 2.0f, 1.0f, "Wall_7_3");
			createWallFromMinMax(sm, root, {120,1,170}, {120,1,140}, 2.0f, 1.0f, "Wall_7_4");
			createWallFromMinMax(sm, root, {120,1,140}, {180,1,140}, 2.0f, 1.0f, "Wall_7_5");

		}






		///Load the first level into the game engine
		///The engine uses Y-UP, Left-handed
		virtual void loadLevel(uint32_t numLevel = 1) {

			VEEngine::loadLevel(numLevel);			//create standard cameras and lights

			VESceneNode* pScene;
			VECHECKPOINTER(pScene = getSceneManagerPointer()->createSceneNode("Level 1", getRoot()));

			//scene models

			VESceneNode* sp1;
			/*VECHECKPOINTER(sp1 = getSceneManagerPointer()->createSkybox("The Sky", "../../media/models/test/sky/cloudy",
				{ "bluecloud_ft.jpg", "bluecloud_bk.jpg", "bluecloud_up.jpg",
					"bluecloud_dn.jpg", "bluecloud_rt.jpg", "bluecloud_lf.jpg" }, pScene));*/
			VECHECKPOINTER(sp1 = getSceneManagerPointer()->createSkybox("The Sky", "../../examples/Game/textures/sky_17_2k/sky_17_cubemap_2k",
				{ "px.png", "nx.png", "py.png",
					"ny.png", "pz.png", "nz.png" }, pScene));
			/*VECHECKPOINTER(sp1 = getSceneManagerPointer()->createSkybox("The Sky", "../../examples/Game/textures/ulukai/ulukai",
				{ "corona_ft.png", "corona_bk.png", "corona_up.png",
					"corona_dn.png", "corona_rt.png", "corona_lf.png" }, pScene));*/

			VESceneNode* e4;
			//VECHECKPOINTER(e4 = getSceneManagerPointer()->loadModel("The Plane", "../../media/models/test/plane", "plane_t_n_s.obj", 0, pScene));
			VECHECKPOINTER(e4 = getSceneManagerPointer()->loadModel("The Plane", "../../examples/Game/textures/Rock035_1K-JPG", "plane_t_n_s.obj", 0, pScene));
			e4->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1.0f, 1000.0f)));

			VEEntity* pE4;
			VECHECKPOINTER(pE4 = (VEEntity*)getSceneManagerPointer()->getSceneNode("The Plane/plane_t_n_s.obj/plane/Entity_0"));
			pE4->setParam(glm::vec4(1000.0f, 1000.0f, 0.0f, 0.0f));


			EntityID g_gui = g_ecs.createEntity();
			g_ecs.addComponent<GUIComponent>(g_gui);


			VESceneNode* Rifle;
			VECHECKPOINTER(Rifle = getSceneManagerPointer()->loadModel(
				"Rifle", 
				"../../examples/Game/models/weapons/uploads_files_4297683_rifleObj", 
				"rifleObj.obj", 
				0, 
				pScene   
			));

			Rifle->setTransform(  glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.7f)));

			g_rifle = g_ecs.createEntity();
			g_ecs.addComponent<PickupComponent>(g_rifle);
			g_ecs.addComponent<WeaponComponent>(g_rifle);
			g_ecs.addComponent<RenderComponent>(g_rifle)->node = Rifle;



			g_player = g_ecs.createEntity();
			g_ecs.addComponent<PlayerComponent>(g_player);
			g_ecs.addComponent<TransformComponent>(g_player)->position = glm::vec3(0.0f, 1.6f, 0.0f);
			g_ecs.addComponent<WeaponStateComponent>(g_player);
			g_ecs.addComponent<RenderComponent>(g_player)->node = Rifle;
			g_ecs.addComponent<HitboxComponent>(g_player)->size = glm::vec3(1.0f, 1.8f, 1.0f);


			std::vector<glm::vec3> potionPositions = {
				{ 0.0f, 1.0f, 40.0f },
				{ 3.0f, 1.0f, 40.0f },
				{ 44.0f, 1.0f, 96.0f },
				{ 46.0f, 1.0f, 96.0f },
				{ 83.0f, 1.0f, 53.0f },
				{ 81.0f, 1.0f, 53.0f },
				{ 79.0f, 1.0f, 53.0f },
				{ 100.0f, 1.0f, 122.0f },
				{ 110.0f, 1.0f, 122.0f },
				{ 110.0f, 1.0f, 128.0f },
				{ 160.0f, 1.0f, 111.0f },
				{ 160.0f, 1.0f, 113.0f },
				{ 160.0f, 1.0f, 115.0f },
				{ 159.5f, 1.0f, 115.0f },
				{ 158.0f, 1.0f, 115.0f },
				{ 155.0f, 1.0f, 32.0f },
				{ 185.0f, 1.0f, 32.0f },
				{ 180.0f, 1.0f, 145.0f },
				{ 178.0f, 1.0f, 145.0f },
				{ 176.0f, 1.0f, 145.0f }
			};

			for (size_t i = 0; i < potionPositions.size(); ++i) {
				VESceneNode* Potion;
				VECHECKPOINTER(Potion = getSceneManagerPointer()->loadModel(
					"Potion" + std::to_string(i), 
					"../../examples/Game/models/items",
					"potion.obj",
					0,
					pScene
				));

				Potion->setTransform(
					glm::translate(glm::mat4(1.0f), potionPositions[i]) *
					/*glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0))*/
					glm::scale(glm::mat4(1.0f), glm::vec3(0.35f))
				);

				EntityID e = g_ecs.createEntity();
				g_ecs.addComponent<PickupComponent>(e);
				g_ecs.addComponent<PotionComponent>(e);
				g_ecs.addComponent<RenderComponent>(e)->node = Potion;
			}






			std::vector<glm::vec3> ammoPositions = {
				{ 30.0f, 1.0f, 25.0f },
				{ 35.0f, 1.0f, 20.0f },
				{ 9.0f, 1.0f, 60.0f },
				{ 10.0f, 1.0f, 60.0f },
				{ 90.0f, 1.0f, 53.0f },
				{ 85.0f, 1.0f, 53.0f },
				{ 105.0f, 1.0f, 128.0f },
				{ 95.0f, 1.0f, 128.0f },
				{ 150.0f, 1.0f, 110.0f },
				{ 150.0f, 1.0f, 118.0f },
				{ 150.0f, 1.0f, 116.0f },
				{ 151.5f, 1.0f, 116.0f },
				{ 153.0f, 1.0f, 116.0f },
				{ 153.0f, 1.0f, 32.0f },
				{ 183.0f, 1.0f, 32.0f },
				{ 185.0f, 1.0f, 165.0f },
				{ 185.0f, 1.0f, 163.0f },
				{ 185.0f, 1.0f, 161.0f }
			};

			for (size_t i = 0; i < ammoPositions.size(); ++i) {
				VESceneNode* ammo;
				VECHECKPOINTER(ammo = getSceneManagerPointer()->loadModel(
					"ammo" + std::to_string(i), 
					"../../examples/Game/models/items/box/ammo",
					"cube.obj",
					0,
					pScene
				));

				ammo->setTransform(
					glm::translate(glm::mat4(1.0f), ammoPositions[i]) *
					/*glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0))*/
					glm::scale(glm::mat4(1.0f), glm::vec3(0.35f))
				);

				EntityID e = g_ecs.createEntity();
				g_ecs.addComponent<PickupComponent>(e);
				g_ecs.addComponent<AmmoComponent>(e);
				g_ecs.addComponent<RenderComponent>(e)->node = ammo;
			}


			std::vector<glm::vec3> keyPositions = {
				{ 9.0f, 1.0f, 77.0f },
				{ 95.0f, 1.0f, 55.0f },
				{ 155.0f, 1.0f, 110.0f },
				{ 123.0f, 1.0f, 163.0f }
			};

			for (size_t i = 0; i < keyPositions.size(); ++i) {
				VESceneNode* key;
				VECHECKPOINTER(key = getSceneManagerPointer()->loadModel(
					"key" + std::to_string(i), 
					"../../examples/Game/models/items/box/key",
					"cube.obj",
					0,
					pScene
				));

				key->setTransform(
					glm::translate(glm::mat4(1.0f), keyPositions[i]) *
					glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1, 0, 0)) *
					glm::scale(glm::mat4(1.0f), glm::vec3(0.55f))
				);

				EntityID e = g_ecs.createEntity();
				g_ecs.addComponent<PickupComponent>(e);
				g_ecs.addComponent<KeyComponent>(e);
				g_ecs.addComponent<RenderComponent>(e)->node = key;
			}

			std::vector<glm::vec3> enemyPositions = {
				{ 10.0f, 0.0f, 75.0f },
				{ 45.0f, 0.0f, 96.0f },
				{ 60.0f, 0.0f, 72.0f },
				{ 75.0f, 0.0f, 60.0f },
				{ 85.0f, 0.0f, 55.0f },
				{ 90.0f, 0.0f, 55.0f },
				//{ 130.0f, 0.0f, 122.0f },
				{ 130.0f, 0.0f, 128.0f },
				{ 130.0f, 0.0f, 118.0f },
				//{ 105.0f, 0.0f, 122.0f },
				{ 95.0f, 0.0f, 122.0f },
				{ 100.0f, 0.0f, 128.0f },
				{ 155.0f, 0.0f, 55.0f },
				{ 170.0f, 0.0f, 40.0f },
				{ 185.0f, 0.0f, 60.0f },
				{ 185.0f, 0.0f, 80.0f },
				{ 130.0f, 0.0f, 160.0f },
				{ 123.0f, 0.0f, 160.0f }
			};

			for (size_t i = 0; i < enemyPositions.size(); ++i) {
				VESceneNode* enemyNode;
				VECHECKPOINTER(enemyNode = getSceneManagerPointer()->loadModel(
					"Enemy" + std::to_string(i),
					"../../examples/Game/models/entities/uploads_files_4148918_Textures",
					"sentry.obj",
					0,
					pScene
				));

				enemyNode->setTransform(
					glm::translate(glm::mat4(1.0f), enemyPositions[i]) *
					glm::scale(glm::mat4(1.0f), glm::vec3(2.0f))
				);

				EntityID enemy = g_ecs.createEntity();
				g_ecs.addComponent<EnemyComponent>(enemy);
				g_ecs.addComponent<HitboxComponent>(enemy)->size = glm::vec3(1.5f, 2.8f, 1.5f);
				g_ecs.addComponent<RenderComponent>(enemy)->node = enemyNode;
			}

			buildLevel(getSceneManagerPointer(), pScene);



			// Here you can run background music
			/*irrklang::ISound* bgMusic = m_irrklangEngine->play2D(
				"../../examples/Game/music/.mp3",
				true,   // loop = true
				false,  // startPaused = false
				true    // track = true
			);

			if (bgMusic)
				bgMusic->setVolume(0.2f);*/

		};
	};




	static std::default_random_engine e{ 12345 };					//Für Zufallszahlen
	static std::uniform_real_distribution<> d{ -10.0f, 10.0f };		//Für Zufallszahlen






	void MyVulkanEngine::registerEventListeners() {

		//VEEngine::registerEventListeners()

		registerEventListener(
			new MyEventListener("MyInput", m_irrklangEngine),
			{
				veEvent::VE_EVENT_KEYBOARD,
				veEvent::VE_EVENT_MOUSEMOVE,
				veEvent::VE_EVENT_MOUSEBUTTON,
				veEvent::VE_EVENT_FRAME_STARTED,
				veEvent::VE_EVENT_FRAME_ENDED
			}
		);

		registerEventListener(
			new PlayerGUI("PlayerHUD"),
			{ veEvent::VE_EVENT_DRAW_OVERLAY }
		);

	}


}

using namespace ve;

int main() {
	bool debug = true;

	MyVulkanEngine mve(veRendererType::VE_RENDERER_TYPE_FORWARD, debug);	//enable or disable debugging (=callback, validation layers)

	mve.initEngine();
	mve.loadLevel(1);
	mve.run();

	return 0;
}
