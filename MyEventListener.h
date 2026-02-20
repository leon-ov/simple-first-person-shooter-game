#pragma once

#include "VEInclude.h"
#include "ECS.h"
#include "irrKlang.h"

namespace ve {

GameState g_gameState = GameState::Playing;

class MyEventListener : public VEEventListener {
protected:

    bool m_usePrevCursorPosition = false; ///<Can I use the previous cursor position for moving the camera?
	bool m_leftButtonClicked = false; ///<Is the left button currently clicked?
	float m_cursorPrevX = 0; ///<Previous X position of cursor
	float m_cursorPrevY = 0; ///<Previous Y position of cursor
    bool m_cursorCaptured = false;
    float m_verticalVelocity = 0.0f;
    bool  m_isOnGround = true;
	VESceneNode* m_pickup = nullptr;
	bool m_hasWeapon = false;
	float m_rotationAngle = 0.0f;
	bool m_weaponAttached = false;
	irrklang::ISoundEngine* m_soundEngine;

	class PlayerGUI;


    virtual bool onKeyboard(veEvent event) override {

        if (event.idata1 == GLFW_KEY_ESCAPE)
		{ //ESC pressed - end the engine
			getEnginePointer()->end();
			return true;
		}

		if (event.idata3 == GLFW_RELEASE)
			return false;

		///create some default constants for the actions
		glm::vec4 translate = glm::vec4(0.0, 0.0, 0.0, 1.0); //total translation
		glm::vec4 rot4 = glm::vec4(1.0); //total rotation around the axes, is 4d !
		float angle = 0.0;
		float rotSpeed = 2.0;

		VECamera *pCamera = getSceneManagerPointer()->getCamera();
		VESceneNode *pParent = pCamera->getParent();

		if (g_gameState == GameState::GameOver)
			return false;


		switch (event.idata1)
		{
		/*case GLFW_KEY_A:
			translate = pCamera->getTransform() * glm::vec4(-1.0, 0.0, 0.0, 1.0); //left
			break;
		case GLFW_KEY_D:
			translate = pCamera->getTransform() * glm::vec4(1.0, 0.0, 0.0, 1.0); //right
			break;
		case GLFW_KEY_W:
			translate = pCamera->getTransform() * glm::vec4(0.0, 0.0, 1.0, 1.0); //forward
			translate.y = 0.0f;
			break;
		case GLFW_KEY_S:
			translate = pCamera->getTransform() * glm::vec4(0.0, 0.0, -1.0, 1.0); //back
			translate.y = 0.0f;
			break;*/

		case GLFW_KEY_W:
			m_moveInput.z =  1.0f;
			break;
		case GLFW_KEY_S:
			m_moveInput.z = -1.0f;
			break;
		case GLFW_KEY_A:
			m_moveInput.x = -1.0f;
			break;
		case GLFW_KEY_D:
			m_moveInput.x =  1.0f;
			break;

		/*case GLFW_KEY_Q:
			translate = glm::vec4(0.0, -1.0, 0.0, 1.0); //down
			break;
		case GLFW_KEY_E:
			translate = glm::vec4(0.0, 1.0, 0.0, 1.0); //up
			break;*/
		case GLFW_KEY_LEFT: //yaw rotation is already in parent space
			angle = rotSpeed * (float)event.dt * -1.0f;
			rot4 = glm::vec4(0.0, 1.0, 0.0, 1.0);
			break;
		case GLFW_KEY_RIGHT: //yaw rotation is already in parent space
			angle = rotSpeed * (float)event.dt * 1.0f;
			rot4 = glm::vec4(0.0, 1.0, 0.0, 1.0);
			break;
		case GLFW_KEY_UP: //pitch rotation is in cam/local space
			angle = rotSpeed * (float)event.dt * 1.0f; //pitch angle
			rot4 = pCamera->getTransform() * glm::vec4(1.0, 0.0, 0.0, 1.0); //x axis from local to parent space!
			break;
		case GLFW_KEY_DOWN: //pitch rotation is in cam/local space
			angle = rotSpeed * (float)event.dt * -1.0f; //pitch angle
			rot4 = pCamera->getTransform() * glm::vec4(1.0, 0.0, 0.0, 1.0); //x axis from local to parent space!
			break;
        case GLFW_KEY_SPACE:
            if (m_isOnGround) {
                m_verticalVelocity = 5.0f;
                m_isOnGround = false;
            }
            break;

		default:
			return false;
		};

		if (pParent == nullptr)
		{
			pParent = pCamera;
		}

		///add the new translation vector to the previous one
		float speed = 6.0f;
		glm::vec3 trans = speed * glm::vec3(translate.x, translate.y, translate.z);
		pParent->multiplyTransform(glm::translate(glm::mat4(1.0f), (float)event.dt * trans));

		///combination of yaw and pitch, both wrt to parent space
		glm::vec3 rot3 = glm::vec3(rot4.x, rot4.y, rot4.z);
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0), angle, rot3);
		pCamera->multiplyTransform(rotate);

        return true;
    }

    virtual bool onMouseMove(veEvent event) override {

		if (g_gameState == GameState::GameOver)
		    return false;


        if (!m_cursorCaptured) {
            GLFWwindow* window = static_cast<GLFWwindow*>(event.ptr);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_cursorCaptured = true;
        }

		float x = event.fdata1;
		float y = event.fdata2;

		if (!m_usePrevCursorPosition)
		{ //can I use the previous cursor position ?
			m_cursorPrevX = x;
			m_cursorPrevY = y;
			m_usePrevCursorPosition = true;
			return true;
		}

		float dx = x - m_cursorPrevX; //motion of cursor in x and y direction
		float dy = y - m_cursorPrevY;

		m_cursorPrevX = x; //remember this for next iteration
		m_cursorPrevY = y;

		VECamera *pCamera = getSceneManagerPointer()->getCamera();
		VESceneNode *pParent = pCamera->getParent();

		float slow = 0.1; //camera rotation speed

		//dx
		float angledx = slow * (float)event.dt * dx;
		glm::vec4 rot4dx = glm::vec4(0.0, 1.0, 0.0, 1.0);
		glm::vec3 rot3dx = glm::vec3(rot4dx.x, rot4dx.y, rot4dx.z);
		glm::mat4 rotatedx = glm::rotate(glm::mat4(1.0), angledx, rot3dx);

		//dy
		float angledy = slow * (float)event.dt * dy; //pitch angle
		glm::vec4 rot4dy = pCamera->getTransform() * glm::vec4(1.0, 0.0, 0.0, 1.0); //x axis from local to parent space!
		glm::vec3 rot3dy = glm::vec3(rot4dy.x, rot4dy.y, rot4dy.z);
		glm::mat4 rotatedy = glm::rotate(glm::mat4(1.0), angledy, rot3dy);

		pCamera->multiplyTransform(rotatedx * rotatedy);

        return true;
    }

    virtual bool onMouseButton(veEvent event) override {

        if (event.idata3 == GLFW_PRESS)
		{ //just pressed a mouse button
			m_usePrevCursorPosition = false;
			if (event.idata1 == GLFW_MOUSE_BUTTON_LEFT)
				m_leftButtonClicked = true;
			return true;
		}

		if (event.idata3 == GLFW_RELEASE)
		{ //just released a mouse button
			m_usePrevCursorPosition = false;
			if (event.idata1 == GLFW_MOUSE_BUTTON_LEFT)
				m_leftButtonClicked = false;
			return true;
		}

        return true;
    }

    virtual void onFrameStarted(veEvent event) override {

		auto player = g_ecs.get<PlayerComponent>(g_player);
		if (player && player->hp <= 0) {
			g_gameState = GameState::GameOver;
		}

		if (player && player->keys >= 4) {
			g_gameState = GameState::Win;
		}

		auto weapon = g_ecs.get<WeaponStateComponent>(g_player);
		auto render = g_ecs.get<RenderComponent>(g_player);

		if (m_leftButtonClicked && player->ammo > 0 && !weapon->isFiring) {
			player->ammo--;
			weapon->isFiring = true;
			weapon->fireTimer = 0.0f;

			spawnBullet();
			playShootSound();
		}

		if (weapon->isFiring) {
			weapon->fireTimer += (float)event.dt;

			float duration = 0.15f; 
			float t = weapon->fireTimer / duration;

			if (t >= 1.0f) {
				weapon->isFiring = false;
				t = 1.0f;
			}

			// smooth kickback
			float kick = sin(t * glm::pi<float>());

			glm::vec3 offset =
				weapon->originalOffset +
				weapon->kickbackOffset * kick;

			render->node->setTransform(
				glm::translate(glm::mat4(1.0f), offset) *
				glm::rotate(glm::mat4(1.0f),
					glm::radians(-5.0f), glm::vec3(1,0,0)) *
				glm::scale(glm::mat4(1.0f), glm::vec3(0.3f))
			);
		}

        VESceneNode* camParent =
            getSceneManagerPointer()->getSceneNode("StandardCameraParent");
        if (!camParent) return;


		glm::vec3 oldPos = camParent->getPosition();

		VECamera* cam = getSceneManagerPointer()->getCamera();

		glm::vec3 forward =
			glm::normalize(glm::vec3(cam->getTransform() * glm::vec4(0,0,1,0)));
		glm::vec3 right =
			glm::normalize(glm::vec3(cam->getTransform() * glm::vec4(1,0,0,0)));

		glm::vec3 moveDir =
			forward * m_moveInput.z +
			right   * m_moveInput.x;

		if (glm::length(moveDir) > 0.0f)
			moveDir = glm::normalize(moveDir);

		float speed = 6.0f;
		glm::vec3 newPos =
			oldPos + moveDir * speed * (float)event.dt;

		newPos.y = oldPos.y;

		camParent->setPosition(newPos);





        float gravity = -9.81f;

        m_verticalVelocity += gravity * (float)event.dt;

        glm::vec3 pos = camParent->getPosition();
        pos.y += m_verticalVelocity * (float)event.dt;

        if (pos.y <= 1.6f) { 
            pos.y = 1.6f;
            m_verticalVelocity = 0.0f;
            m_isOnGround = true;
        }

        camParent->setPosition(pos);




		auto* playerHitbox = g_ecs.get<HitboxComponent>(g_player);
		if (!playerHitbox) return;

		for (EntityID e : g_ecs.entities()) {

			if (e == g_player) continue;

			auto* wallHitbox = g_ecs.get<HitboxComponent>(e);
			auto* wallRender = g_ecs.get<RenderComponent>(e);

			if (!wallHitbox || !wallRender) continue;

			glm::vec3 wallPos = wallRender->node->getPosition();

			if (aabbIntersect(
				newPos, playerHitbox->size,
				wallPos, wallHitbox->size
			)) {
				camParent->setPosition(oldPos);
				break;
			}
		}

		m_moveInput = glm::vec3(0.0f);

			auto camPos = camParent->getPosition();

			// ===== BULLET UPDATE =====
			for (EntityID e : g_ecs.entities()) {

				auto bullet = g_ecs.get<BulletComponent>(e);
				auto render = g_ecs.get<RenderComponent>(e);

				if (!bullet || !render) continue;

				VESceneNode* node = render->node;
				glm::vec3 pos = node->getPosition();

				glm::vec3 step = bullet->direction * bullet->speed * (float)event.dt;
				pos += step;

				bullet->traveled += glm::length(step);
				node->setPosition(pos);

				if (bullet->traveled >= bullet->maxDistance) {
					getSceneManagerPointer()->deleteSceneNodeAndChildren(node->getName());
					g_ecs.deleteEntity(e);
				}

				for (EntityID target : g_ecs.entities()) {

					auto enemy = g_ecs.get<EnemyComponent>(target);
					auto hitbox = g_ecs.get<HitboxComponent>(target);
					auto enemyRender = g_ecs.get<RenderComponent>(target);

					if (!enemy || !hitbox || !enemyRender) continue;

					glm::vec3 epos = enemyRender->node->getPosition();
					glm::vec3 min = epos - hitbox->size * 0.5f;
					glm::vec3 max = epos + hitbox->size * 0.5f;

					if (aabbHit(pos, min, max)) {

						enemy->hp -= bullet->damage;

						//printf("Enemy %d HP: %d\n", e, enemy->hp);

						getSceneManagerPointer()->deleteSceneNodeAndChildren(render->node->getName());
						g_ecs.deleteEntity(e);

						if (enemy->hp <= 0) {
							playDestroyingSound();
							getSceneManagerPointer()->deleteSceneNodeAndChildren(enemyRender->node->getName());
							g_ecs.deleteEntity(target);
						}

						break;
					}
				}

				if (bullet->owner == BulletOwner::Enemy) {

					auto playerHitbox = g_ecs.get<HitboxComponent>(g_player);
					if (playerHitbox) {

						glm::vec3 ppos = camParent->getPosition();
						glm::vec3 min = ppos - playerHitbox->size * 0.5f;
						glm::vec3 max = ppos + playerHitbox->size * 0.5f;

						if (aabbHit(pos, min, max)) {

							player->hp -= bullet->damage;

							playDamageSound();

							getSceneManagerPointer()->deleteSceneNodeAndChildren(node->getName());
							g_ecs.deleteEntity(e);
						}
					}
				}

			}

			for (EntityID e : g_ecs.entities()) {

				auto enemy = g_ecs.get<EnemyComponent>(e);
				auto enemyRender = g_ecs.get<RenderComponent>(e);

				if (!enemy || !enemyRender) continue;

				glm::vec3 enemyPos = enemyRender->node->getPosition();
				glm::vec3 playerPos = camParent->getPosition();

				glm::vec3 toPlayer = playerPos - enemyPos;
				float dist = glm::length(toPlayer);

				// ===== ROTATE TO PLAYER =====
				if (dist < enemy->visionRange) {
					glm::vec3 dir = glm::normalize(toPlayer);

					float angle = atan2(-dir.x, -dir.z);
					enemyRender->node->setTransform(
						glm::translate(glm::mat4(1.0f), enemyPos) *
						glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0,1,0)) *
						glm::scale(glm::mat4(1.0f), glm::vec3(1.8f))
					);
				}

				// ===== ATTACK =====
				enemy->shootTimer += (float)event.dt;

				if (dist < enemy->attackRange &&
					enemy->shootTimer >= enemy->shootCooldown) {

					enemy->shootTimer = 0.0f;
					spawnEnemyBullet(enemyPos, toPlayer);
					playCanonSound();
				}
			}



			for (EntityID e : g_ecs.entities()) {

				auto pickup = g_ecs.get<PickupComponent>(e);
				auto wp = g_ecs.get<WeaponComponent>(e);
				auto render = g_ecs.get<RenderComponent>(e);

				if (!pickup /*|| !weapon*/ || !render) continue;
				if (pickup->collected) continue;

				// rotation
				VESceneNode* node = render->node;

				glm::vec3 pos = node->getPosition();

				glm::mat4 T  = glm::translate(glm::mat4(1.0f), pos);
				glm::mat4 Ti = glm::translate(glm::mat4(1.0f), -pos);
				glm::mat4 R  = glm::rotate(
					glm::mat4(1.0f),
					(float)event.dt,
					glm::vec3(0,1,0)
				);

				node->setTransform(T * R * Ti * node->getTransform());
		

				glm::vec3 pickupPos = render->node->getPosition();
				float dist = glm::length(camPos - pickupPos);

				if (dist < 1.5f) {
					 // ===== WEAPON =====
					if (auto weapon = g_ecs.get<WeaponComponent>(e)) {

						pickup->collected = true;
						weapon->equipped = true;
						player->hasWeapon = true;

						player->ammo = 75;

						VECamera* cam = getSceneManagerPointer()->getCamera();
						cam->addChild(render->node);

						playPickupSound();


						render->node->setTransform(
							glm::translate(glm::mat4(1.0f),
								glm::vec3(0.35f, -0.35f, 0.8f)) *
							glm::rotate(glm::mat4(1.0f),
								glm::radians(-5.0f), glm::vec3(1,0,0)) *
							glm::scale(glm::mat4(1.0f), glm::vec3(0.3f))
						);
					}

					// ===== POTION =====
					if (auto potion = g_ecs.get<PotionComponent>(e) && player->hp < 100) {

						if (!pickup->collected) {    

							player->hp += 15;
							if(player->hp >= 100) player->hp = 100;

							pickup->collected = true;    
							playPickupSound();
							getSceneManagerPointer()->deleteSceneNodeAndChildren(render->node->getName());
							g_ecs.deleteEntity(e);
						}

					}

					// ===== AMMO =====
					if (auto ammo = g_ecs.get<AmmoComponent>(e) && player->ammo < 150 && player->hasWeapon) {

						if (!pickup->collected) {   

							player->ammo += 30;
							if(player->ammo >= 150) player->ammo = 150;

							pickup->collected = true;   
							playPickupSound();
							getSceneManagerPointer()->deleteSceneNodeAndChildren(render->node->getName());
							g_ecs.deleteEntity(e);
						}

					}

					// ===== KEY =====
					if (auto key = g_ecs.get<KeyComponent>(e)) {

						if (!pickup->collected) {   

							player->keys += 1;

							pickup->collected = true;   
							playPickupSound();
							getSceneManagerPointer()->deleteSceneNodeAndChildren(render->node->getName());
							g_ecs.deleteEntity(e);
						}

					}
				}
			}

		

    }


public:
    MyEventListener(std::string name, irrklang::ISoundEngine* engine)
        : VEEventListener(name), m_soundEngine(engine) {}

    virtual ~MyEventListener() {}

	void playPickupSound() {
        if (m_soundEngine)
            m_soundEngine->play2D("../../examples/Game/sfx/weapon_pickUp.mp3", false);
    }

	void playShootSound() {
		if (m_soundEngine)
			m_soundEngine->play2D("../../examples/Game/sfx/Rifle_shoot.mp3", false);
	}

	void playTripleShootSound() {
		if (m_soundEngine)
			m_soundEngine->play2D("../../examples/Game/sfx/triple_shot.mp3", false);
	}

	void playCanonSound() {
		if (m_soundEngine)
			m_soundEngine->play2D("../../examples/Game/sfx/canon.mp3", false);
	}

	void playDamageSound() {
		if (m_soundEngine)
			m_soundEngine->play2D("../../examples/Game/sfx/gettingDamage.mp3", false);
	}

	void playDestroyingSound() {
		if (m_soundEngine)
			m_soundEngine->play2D("../../examples/Game/sfx/destroyingCanon.mp3", false);
	}

	void spawnBullet() {

		VECamera* cam = getSceneManagerPointer()->getCamera();
		VESceneNode* camParent = cam->getParent();

		glm::vec3 camPos = camParent->getPosition();

		glm::vec3 dir = glm::normalize(
			glm::vec3(cam->getTransform() * glm::vec4(0, 0, 1, 0))
		);

		VESceneNode* bulletNode =
			getSceneManagerPointer()->loadModel(
				"Bullet_" + std::to_string(rand()),
				"../../examples/Game/models/bullet",
				"bullet.obj",
				0,
				getSceneManagerPointer()->getRootSceneNode()
			);

		bulletNode->setTransform(
			glm::translate(glm::mat4(1.0f), camPos + dir * 0.5f) *
			glm::scale(glm::mat4(1.0f), glm::vec3(1.0f))
		);

		// ===== ECS =====
		EntityID bullet = g_ecs.createEntity();

		auto* bc = g_ecs.addComponent<BulletComponent>(bullet);
		bc->direction = dir;
		bc->owner = BulletOwner::Player;

		g_ecs.addComponent<RenderComponent>(bullet)->node = bulletNode;
	}

	void spawnEnemyBullet(glm::vec3 pos, glm::vec3 toPlayer) {

		glm::vec3 dir = glm::normalize(toPlayer);

		VESceneNode* node =
			getSceneManagerPointer()->loadModel(
				"EnemyBullet_" + std::to_string(rand()),
				"../../examples/Game/models/bullet",
				"bullet.obj",
				0,
				getSceneManagerPointer()->getRootSceneNode()
			);

		node->setTransform(
			glm::translate(glm::mat4(1.0f), pos + dir * 1.0f) *
			glm::scale(glm::mat4(1.0f), glm::vec3(10.0f))
		);

		EntityID bullet = g_ecs.createEntity();
		auto* bc = g_ecs.addComponent<BulletComponent>(bullet);
		bc->direction = dir;
		bc->owner = BulletOwner::Enemy;

		g_ecs.addComponent<RenderComponent>(bullet)->node = node;
	}


	bool aabbHit(glm::vec3 p, glm::vec3 a, glm::vec3 b) {
		return
			p.x > a.x && p.x < b.x &&
			p.y > a.y && p.y < b.y &&
			p.z > a.z && p.z < b.z;
	}

	bool aabbIntersect(
		const glm::vec3& aPos, const glm::vec3& aSize,
		const glm::vec3& bPos, const glm::vec3& bSize
	) {
		glm::vec3 aHalf = aSize * 0.5f;
		glm::vec3 bHalf = bSize * 0.5f;

		return
			fabs(aPos.x - bPos.x) <= (aHalf.x + bHalf.x) &&
			fabs(aPos.y - bPos.y) <= (aHalf.y + bHalf.y) &&
			fabs(aPos.z - bPos.z) <= (aHalf.z + bHalf.z);
	}




};

class PlayerGUI : public VEEventListener {
	public:
		PlayerGUI(std::string name) : VEEventListener(name) {}

		virtual void onDrawOverlay(veEvent event) override {
			VESubrender_Nuklear* overlay = (VESubrender_Nuklear*)getEnginePointer()->getRenderer()->getOverlay();
			if (!overlay) return;

			struct nk_context* ctx = overlay->getContext();
			if (!ctx) return;

			auto player = g_ecs.get<PlayerComponent>(g_player);
			if (!player) return;

			char buffer[32];


			if (nk_begin(ctx, "", nk_rect(0, 0, 200, 170), NK_WINDOW_BORDER )) {
				nk_layout_row_dynamic(ctx, 25, 1);
				//sprintf(buffer, "HP: %d/%d", player->hp, player->maxHP);
				sprintf(buffer, "HP: %d", player->hp);
				nk_label(ctx, buffer, NK_TEXT_LEFT);

				nk_layout_row_dynamic(ctx, 25, 1);
				sprintf(buffer, "Ammo: %d", player->ammo);
				nk_label(ctx, buffer, NK_TEXT_LEFT);

				nk_layout_row_dynamic(ctx, 25, 1);
				sprintf(buffer, "Keys: %d/4", player->keys);
				nk_label(ctx, buffer, NK_TEXT_LEFT);
			}

			nk_end(ctx);

			VERenderer* renderer = getEnginePointer()->getRenderer();
			float screen_w = (float)renderer->getSwapChainExtent().width;
			float screen_h = (float)renderer->getSwapChainExtent().height;

			float win_w = 32.0f;
			float win_h = 32.0f;
			float win_x = (screen_w * 0.5f) - (win_w * 0.5f);
			float win_y = (screen_h * 0.5f) - (win_h * 0.5f);

			nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(nk_rgba(0,0,0,0)));
			nk_style_push_vec2(ctx, &ctx->style.window.padding, nk_vec2(0,0));

			if (nk_begin(ctx, "CrosshairCenter", nk_rect(win_x, win_y, win_w, win_h),
						NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR ))
			{
				struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
				
				float cx = win_x + (win_w * 0.5f);
				float cy = win_y + (win_h * 0.5f);
				
				float size = 6.0f;   
				float thickness = 2.0f; 
				struct nk_color color = nk_rgb(255, 255, 255);

				nk_fill_rect(canvas, 
					nk_rect(cx - size, cy - (thickness * 0.5f), size * 2.0f, thickness), 
					0.0f, color);

				nk_fill_rect(canvas, 
					nk_rect(cx - (thickness * 0.5f), cy - size, thickness, size * 2.0f), 
					0.0f, color);
			}
			nk_end(ctx);

			nk_style_pop_vec2(ctx); 
			nk_style_pop_style_item(ctx);

			if (g_gameState == GameState::GameOver) {

				if (nk_begin(ctx, "GameOver",
					nk_rect(400, 250, 400, 200),
					NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
				{
					nk_layout_row_dynamic(ctx, 40, 1);
					nk_label(ctx, "GAME OVER", NK_TEXT_CENTERED);

					nk_label(ctx, "Press ESC to close the game.", NK_TEXT_CENTERED);

					/*nk_layout_row_dynamic(ctx, 30, 1);
					if (nk_button_label(ctx, "Exit")) {
						getEnginePointer()->end();
					}*/
				}
				nk_end(ctx);
				return;
			} else if (g_gameState == GameState::Win){
				if (nk_begin(ctx, "Win!",
					nk_rect(400, 250, 400, 200),
					NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
				{
					nk_layout_row_dynamic(ctx, 40, 1);
					nk_label(ctx, "Win!", NK_TEXT_CENTERED);
					nk_label(ctx, "Congrats! You finished the game.", NK_TEXT_CENTERED);
					nk_label(ctx, "Press ESC to close the game.", NK_TEXT_CENTERED);
				}
				nk_end(ctx);
				return;
			}


		}
	};

} // namespace ve
