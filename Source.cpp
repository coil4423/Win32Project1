#include <math.h>
#include <DxLib.h>

//�t���[�����[�g��ς��邽�߂̃p�����[�^
static int mStartTime; //����J�n����
static int mCount; //�J�E���^
static float mFps; //fps
static const int N = 30; //���ς��Ƃ�T���v����
static const int FPS = 30; //�ݒ肵��FPS

//�֐��̒�`
bool Update() {
	//1�t���[���ڂȂ玞�����L��
	if (mCount == 0) {
		mStartTime = GetNowCount();
	}
	//60�t���[���ڂȂ畽�ς��v�Z����
	if (mCount == N) {
		int t = GetNowCount();
		mFps = 1000.f / ((t - mStartTime) / (float)N);
		mCount = 0;
		mStartTime = t;
	}
	mCount++;
	return true;
}

void Draw() {
	DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS %.1f", mFps);
}

void Wait() {
	//������������
	int tookTime = GetNowCount() - mStartTime;
	//�҂ׂ�����
	int waitTime = mCount * 1000 / FPS - tookTime;
	if (waitTime > 0) {
		//�ҋ@
		Sleep(waitTime);
	}
}

int quake(int);

void Scene(int*, int*);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(TRUE);
	DxLib_Init();
	//���[�h�\��
	DrawFormatString(240, 240, GetColor(255, 255, 255), "Now Loading...");

	//����ʂɕ`��w��
	SetDrawScreen(DX_SCREEN_BACK);



	//�����ʒu
	int playerX = 32 * 10 - 16, playerY = 32 * 11;
	int monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9
	
	//�e��p�����[�^�ݒ�
	int variationX = -32 * 2; // variationX = -32 * 2
	int x, y;
	int acceleY = 0, acceleG = 0;
	const int playerWidth = 32, playerHeight = 48;
	const int monsterWidth = 48, monsterHeight = 96;
	const int monsterReachX = 72;
	const int mapWidth = 200, mapHeight = 15;
	int playerWalkCount = 0;
	int monsterActionCount = 0;
	int fireCount = -1;
	int quakeY = 0;
	int count = 0;
	int sceneCount = -1; //sceneCount = -1

	//�e��t���O�ݒ�
	bool jumpFlag = false;
	bool callFlag = false;
	bool playerLeftDirection = true;
	bool playerRightDirection = false;
	bool monsterLeftDirection = true;
	bool monsterRightDirection = false;
	bool monsterAction = false;

	//�}�b�v�f�[�^�ǂݎ��
	FILE *fp;
	fopen_s(&fp, "Data/map.csv", "r");
	int map[mapHeight][mapWidth];
	int syokika[mapHeight][mapWidth];
	for (int i = 0; i < mapHeight; i++) {
		for (int j = 0; j < mapWidth; j++) {
			fscanf_s(fp, "%d,", &map[i][j]);
			syokika[i][j] = map[i][j];
		}
	}



	//�L�[�̏�Ԃ��i�[����z���p��
	char key[256];

	//�摜��p��
	int background = LoadGraph("Data/background.png");
	int ground1 = LoadGraph("Data/ground1.png");
	int ground2 = LoadGraph("Data/ground2.png");
	int rock = LoadGraph("Data/rock.png");
	int mud = LoadGraph("Data/mud.png");
	int net = LoadGraph("Data/net.png");
	int fire = LoadGraph("Data/fire.png");
	int playerLeftStand = LoadGraph("Data/playerB_003.png");
	int playerLeftWalk = LoadGraph("Data/playerB_002.png");
	int playerRightStand = LoadGraph("Data/playerB_001.png");
	int playerRightWalk = LoadGraph("Data/playerB_000.png");
	int monsterLeftStand = LoadGraph("Data/monster_000.png");
	int monsterRightStand = LoadGraph("Data/monster_001.png");
	int monsterRightUpHand = LoadGraph("Data/monster_004.png");
	int monsterRightDownHand = LoadGraph("Data/monster_002.png");
	int sword = LoadGraph("Data/sword.png");
	int mask1 = LoadGraph("Data/mask.png");
	int mask2 = LoadGraph("Data/mask2.png");
	int mob1 = LoadGraph("Data/mob1.png");
	int mob2 = LoadGraph("Data/mob2.png");



	//�Q�[������
	while (1) {
		//sceneCount��0�ȏ�ŃQ�[���������n�܂�
		if (sceneCount >= 0) {
			ProcessMessage();
			Update();

			//�S�ẴL�[�̓��͂��Ď�����
			GetHitKeyStateAll(key);

			//�I��
			if (key[KEY_INPUT_Q] == 1) {
				break;
			}

			//��ʂ̕`�������
			ClearDrawScreen();



			//�w�i�`��
			DrawGraph(0, 0, background, TRUE);



			//�}�b�v����
			for (y = 0; y < mapHeight; y++) {
				for (x = 0; x < mapWidth; x++) {
					if (map[y][x] == 1) {
						DrawGraph(x * 32 + variationX, y * 32 + quakeY, ground1, TRUE);
					}
					else if (map[y][x] == 2) {
						DrawGraph(x * 32 + variationX, y * 32 + quakeY, ground2, TRUE);
					}
					else if (map[y][x] == 3) {
						DrawGraph(x * 32 + variationX, y * 32 + quakeY, rock, TRUE);
					}
					else if (map[y][x] == 4) {
						DrawGraph(x * 32 + variationX, y * 32 + quakeY, mud, TRUE);
					}
					else if (map[y][x] == 5) {
						DrawGraph(x * 32 + variationX, y * 32 + quakeY, net, TRUE);
					}
					else if (map[y][x] == 6) {
						DrawGraph(x * 32 + variationX, y * 32 + quakeY, fire, TRUE);
					}
				}
			}

			//���u�L�����`��
			for (x = 0; x < 8; x++) {
				DrawGraph(32 * (199 - 2 * x) + variationX, 32 * 11 - 16, mob1, TRUE);
				DrawGraph(32 * (198 - 2 * x) + variationX, 32 * 11 - 16, mob2, TRUE);
			}




			//�v���C���[����
			//�}�b�v�X�N���[��
			//���L�[��������āA�v���C���[�̍���ƍ����ɉ����Ȃ���΍��ɐi��
			if (key[KEY_INPUT_LEFT] == 1) {
				if (map[(int)(playerY / 32)][(int)(playerX - 1 - variationX) / 32] == 0 && map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - 1 - variationX) / 32)] == 0) {
					variationX += 2;
				}
				playerLeftDirection = true;
				playerRightDirection = false;
				playerWalkCount += 1;
			}
			//�E�L�[��������āA�v���C���[�̉E��ƉE���ɉ����Ȃ���ΉE�ɐi��
			if (key[KEY_INPUT_RIGHT] == 1) {
				if (map[(int)(playerY / 32)][(int)(playerX + playerWidth - variationX) / 32] == 0 && map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] == 0 && -variationX < 32 * 180) {
					variationX += -2;
				}
				playerRightDirection = true;
				playerLeftDirection = false;
				playerWalkCount += 1;
			}

			//�W�����v����
			//�W�����v���͉������ɉ���
			if (jumpFlag == true) {
				playerY += acceleY + acceleG;
				acceleG += 1;
			}
			//�v���C���[�����ɂ�����t���O��߂��A���ւ̂߂荞�݂�߂�
			if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY + playerHeight - 1) / 32)][(int)(playerX + playerWidth - 1 - variationX) / 32] != 0) {
				jumpFlag = false;
				while (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] != 0) {
					playerY += -1;
				}
			}
			//�v���C���[�����ɂ��鎞�ɏ�L�[�������ꂽ��A�t���O�𗧂ĉ����x��ݒ�
			if (key[KEY_INPUT_UP] == 1 && jumpFlag == false) {
				jumpFlag = true;
				acceleY = -11;
				acceleG = 0;
			}
			//��L�[��Z�������Ə��W�����v
			if (jumpFlag == true && key[KEY_INPUT_UP] == 0) {
				acceleG += 1;
			}
			//���ꂩ�痎�����Ƃ�
			if (jumpFlag == false && map[(int)((playerY + playerHeight) / 32)][(int)((playerX - variationX) / 32)] == 0 && map[(int)((playerY + playerHeight) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] == 0) {
				jumpFlag = true;
				acceleY = 0;
				acceleG = 0;
			}
			//�v���C���[���V��ɒ�������A�V��ւ̂߂荞�݂�߂�
			if (map[(int)((playerY - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY - 1) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] != 0) {
				while (map[(int)((playerY - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY - 1) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] != 0) {
					playerY += 1;
				}
			}

			//�v���C���[�A�N�V��������
			if (key[KEY_INPUT_X] == 1) {
				//�y���^��
				//�v���C���[�̉E���̓y���^��
				if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] == 4) {
					map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] = 0;
					for (x = 0; x < mapWidth; x++) {
						if (map[12][x] == 0) {
							map[12][x] = 2;
							break;
						}
					}
				}
				//�v���C���[�̍����̓y���^��
				else if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX - 1) / 32)] == 4) {
					map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX - 1) / 32)] = 0;
					for (x = 0; x < mapWidth; x++) {
						if (map[12][x] == 0) {
							map[12][x] = 2;
							break;
						}
					}
				}

				//�w偂̑���R�₷
				//�v���C���[�̉E���̒w偂̑���R�₷
				if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] == 5) {
					fireCount = 0;
				}
				//�v���C���[�̍����̒w偂̑���R�₷
				else if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX - 1) / 32)] == 5) {
					fireCount = 0;
				}

				//�����X�^�[�����Ŏh��
				if (sceneCount == 26 && ((monsterX + variationX <= playerX && monsterX + monsterWidth + variationX > playerX) || (monsterX + variationX <= playerX + playerWidth && monsterX + monsterWidth + variationX > playerX + playerWidth))) {
					count = 0;
					sceneCount = 0;
					ClearDrawScreen();
					//�G���f�B���O
					while (1) {
						ProcessMessage();

						SetFontSize(64);
						DrawFormatString(32 * 6, 32 * 3, GetColor(255, 0, 0), "THE END");
						SetFontSize(16);
						DrawFormatString(32 * 7, 32 * 5, GetColor(255, 0, 0), "Monster Was Killed");

						if (sceneCount == 1) {
							DrawFormatString(32 * 7, 32 * 6, GetColor(0, count, count / 2), "But People Was Saved ");
						}

						if (sceneCount == 2) {
							DrawFormatString(32 * 8, 32 * 8, GetColor(count, count, count), "\"R\"estart\n\"P\"rologue skipped restart");

							//���X�^�[�g
							if (CheckHitKey(KEY_INPUT_R) == 1) {
								//�ϐ���������
								//�����ʒu
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

								//�e��p�����[�^�ݒ�
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = -1;

								//�e��t���O�ݒ�
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//�}�b�v������
								for (y = 0; y < mapHeight; y++) {
									for (x = 0; x < mapWidth; x++) {
										map[y][x] = syokika[y][x];
									}
								}

								break;
							}

							//�v�����[�O�Ƃ΂��ă��X�^�[�g
							if (CheckHitKey(KEY_INPUT_P) == 1) {
								//�ϐ���������
								//�����ʒu
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

																	   //�e��p�����[�^�ݒ�
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = 0;

								//�e��t���O�ݒ�
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//�}�b�v������
								for (y = 0; y < mapHeight; y++) {
									for (x = 0; x < mapWidth; x++) {
										map[y][x] = syokika[y][x];
									}
								}

								break;
							}
						}


						if (count == 255) {
							count = 0;
							if (sceneCount < 2) {
								sceneCount += 1;
							}
						}
						
						count++;
						
						ScreenFlip();
					}
				}

				//���l�����Ŏh��
				if (sceneCount == 26 && -variationX >= 32 * 170) {
					count = 0;
					sceneCount = 0;
					ClearDrawScreen();
					//�G���f�B���O
					while (1) {
						ProcessMessage();

						SetFontSize(64);
						DrawFormatString(32 * 6, 32 * 3, GetColor(255, 0, 0), "THE END");
						SetFontSize(16);
						DrawFormatString(32 * 7, 32 * 5, GetColor(255, 0, 0), "People Was Killed");

						if (sceneCount == 1) {
							DrawFormatString(32 * 7, 32 * 6, GetColor(0, count, count / 2), "But Monster Was Saved ");
						}

						if (sceneCount == 2) {
							DrawFormatString(32 * 8, 32 * 8, GetColor(count, count, count), "\"R\"estart\n\"P\"rologue skipped restart");

							//���X�^�[�g
							if (CheckHitKey(KEY_INPUT_R) == 1) {
								//�ϐ���������
								//�����ʒu
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

								//�e��p�����[�^�ݒ�
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = -1;

								//�e��t���O�ݒ�
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//�}�b�v������
								for (y = 0; y < mapHeight; y++) {
									for (x = 0; x < mapWidth; x++) {
										map[y][x] = syokika[y][x];
									}
								}


								break;
							}

							//���X�^�[�g
							if (CheckHitKey(KEY_INPUT_P) == 1) {
								//�ϐ���������
								//�����ʒu
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

																	   //�e��p�����[�^�ݒ�
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = 0;

								//�e��t���O�ݒ�
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//�}�b�v������
								for (y = 0; y < mapHeight; y++) {
									for (x = 0; x < mapWidth; x++) {
										map[y][x] = syokika[y][x];
									}
								}

								break;
							}
						}


						if (count == 255) {
							count = 0;
							if (sceneCount < 2) {
								sceneCount += 1;
							}
						}

						count++;

						ScreenFlip();
					}
				}
			}

			//�w偂̑����R����A�j���[�V����
			if (fireCount >= 0 && fireCount < 10) {
				for (y = 0; y < mapHeight; y++) {
					if (map[y][(int)((playerX + playerWidth - variationX) / 32)] == 5) {
						map[y][(int)((playerX + playerWidth - variationX) / 32)] = 6;
					}
					else if (map[y][(int)((playerX - variationX - 1) / 32)] == 5) {
						map[y][(int)((playerX - variationX - 1) / 32)] = 6;
					}
				}
				fireCount += 1;
			}
			else if (fireCount >= 10) {
				for (y = 0; y < mapHeight; y++) {
					for (x = 0; x < mapWidth; x++) {
						if (map[y][x] == 6) {
							map[y][x] = 0;
						}
					}
				}
				fireCount = -1;
			}

			//���`�� scene03���߂������\��
			if (sceneCount < 4) {
				DrawGraph(32 * 22 + variationX, 32 * 11, sword, TRUE);
			}



			//�����X�^�[����
			//�����X�^�[�Ăяo���A�N�V����
			if ((monsterX + variationX > playerX - 10 && monsterX + variationX < playerX - 6) || monsterX > 32 * 177) {
				callFlag = false;
			}
			if (key[KEY_INPUT_C] == 1 && variationX <= -32 * 29 && variationX > -32 * 168) {
				callFlag = true;
			}


			//�Ăяo������
			if (callFlag == true) {
				//�����X�^�[���v���C���[��荶�ɂ���ΉE�ɕ���
				if (monsterX + variationX < playerX - 6) {
					//�E�ɕǂ��Ȃ���ΉE�ɕ���
					if (map[(int)(monsterY / 32)][(int)((monsterX + monsterWidth) / 32)] == 0 && map[(int)((monsterY + monsterHeight - 1) / 32)][(int)((monsterX + monsterWidth) / 32)] == 0) {
						monsterX += 3;
						monsterRightDirection = true;
						monsterLeftDirection = false;
					}
					//�E�ɉ󂹂�₪�����
					else if (map[(int)((monsterY + monsterHeight - 1) / 32)][(int)((monsterX + monsterWidth) / 32)] == 3) {
						monsterAction = true;
					}
					//��������Ɠn�炸�Ɍジ����
					if (map[(int)((monsterY + monsterHeight) / 32)][(int)((monsterX + monsterWidth) / 32)] == 0) {
						monsterX += -9;
						callFlag = false;
						//scene13��`��
						sceneCount = 13;
					}
				}
				//�����X�^�[���v���C���[���E�ɂ���΍��ɕ���
				else if (monsterX + variationX > playerX - 10) {
					//���ɕǂ��Ȃ���΍��ɕ���
					if (map[(int)(monsterY / 32)][(int)((monsterX - 1) / 32)] == 0 && map[(int)((monsterY + monsterHeight - 1) / 32)][(int)((monsterX - 1) / 32)] == 0) {
						monsterX += -3;
						monsterLeftDirection = true;
						monsterRightDirection = false;
					}
				}
			}

			//�����X�^�[�`��
			//�E�����ɕ`��
			if (monsterRightDirection == true) {
				//����󂷃A�j���[�V����
				if (monsterAction == true) {
					if (monsterActionCount < 10) {
						DrawGraph(monsterX + variationX, monsterY, monsterRightUpHand, TRUE);
					}
					else if (monsterActionCount >= 10) {
						DrawGraph(monsterX + variationX, monsterY, monsterRightDownHand, TRUE);
						quakeY = quake(quakeY);
						if (monsterActionCount >= 20) {
							monsterActionCount = -1;
							monsterAction = false;
							//�����X�^�[�ׂ̗̊������
							for (int i = monsterHeight; i > 0; i += -32) {
								map[(int)((monsterY + i - 1) / 32)][(int)((monsterX + monsterReachX) / 32)] = 0;
							}
						}
					}
					monsterActionCount += 1;
				}
				//����,����
				else {
					DrawGraph(monsterX + variationX, monsterY, monsterRightStand, TRUE);
				}
			}
			//�������ɕ`��
			else if (monsterLeftDirection == true) {
				DrawGraph(monsterX + variationX, monsterY, monsterLeftStand, TRUE);
			}



			//�v���C���[�`��
			//�������ɕ���
			if (playerLeftDirection == true) {
				//���L�[�������ꂽ��E�����ɕ����A�j���[�V����
				if (key[KEY_INPUT_LEFT] == 1) {
					if (playerWalkCount < 5) {
						DrawGraph(playerX, playerY, playerLeftStand, TRUE);
					}
					else if (playerWalkCount >= 5) {
						DrawGraph(playerX, playerY, playerLeftWalk, TRUE);
						if (playerWalkCount >= 9) {
							playerWalkCount = -1;
						}
					}
				}
				//���L�[��������ĂȂ������獶�����ɗ��Î~��
				else if (key[KEY_INPUT_LEFT] == 0) {
					DrawGraph(playerX, playerY, playerLeftStand, TRUE);
					playerWalkCount = -1;
				}
			}
			//�E�����ɕ���
			else if (playerRightDirection == true) {
				//�E�L�[�������ꂽ��E�����ɕ����A�j���[�V����
				if (key[KEY_INPUT_RIGHT] == 1) {
					if (playerWalkCount < 5) {
						DrawGraph(playerX, playerY, playerRightStand, TRUE);
					}
					else if (playerWalkCount >= 5) {
						DrawGraph(playerX, playerY, playerRightWalk, TRUE);
						if (playerWalkCount >= 9) {
							playerWalkCount = -1;
						}
					}
				}
				//�E�L�[��������ĂȂ�������E�����ɗ��Î~��
				else if (key[KEY_INPUT_RIGHT] == 0) {
					DrawGraph(playerX, playerY, playerRightStand, TRUE);
					playerWalkCount = -1;
				}
			}


			//�Èł�`��
			if (-variationX < 32 * 156) {
				DrawGraph(0, playerY - 32 * 12, mask1, TRUE);
			}
			if (-variationX >= 32 * 156 && -variationX < 32 * 167) {
				DrawGraph(0, playerY - 32 * 12, mask2, TRUE);
			}


			//�V�[���Ǘ�
			//scene01
			//�V�[���̏ꏊ�ɓ�������`��
			if (-variationX >= 0 && -variationX < 32 * 8 && sceneCount == 0) {
				sceneCount = 1;
			}
			//scene02
			//�V�[���̏ꏊ����o����i�v�ɕ`�悵�Ȃ�
			if (-variationX >= 32 * 8 && -variationX < 32 * 10) {
				sceneCount = 2;
			}

			//scene03
			if (-variationX >= 32 * 10 && -variationX <= 32 * 13 && sceneCount == 2) {
				sceneCount = 3;
			}
			//scene04
			if (-variationX >= 32 * 13 && -variationX < 32 * 17) {
				sceneCount = 4;
			}

			//scene05
			if (-variationX >= 32 * 17 && -variationX < 32 * 22 && sceneCount == 4) {
				sceneCount = 5;
			}
			//scene06
			if (-variationX >= 32 * 22 && -variationX < 32 * 29) {
				sceneCount = 6;
			}

			//scene07
			if (-variationX >= 32 * 29 && -variationX < 32 * 36 && sceneCount == 6) {
				sceneCount = 7;
			}
			//scene08
			if (-variationX >= 32 * 36 && -variationX < 32 * 43) {
				sceneCount = 8;
			}

			//scene09
			if (-variationX >= 32 * 43 && -variationX < 32 * 48 && sceneCount == 8 && monsterX <= 32 * 48) {
				sceneCount = 9;
			}
			//scene10
			if (-variationX >= 32 * 48 && -variationX < 32 * 50) {
				sceneCount = 10;
			}

			//scene11
			if (-variationX >= 32 * 50 && -variationX < 32 * 57 && sceneCount == 10) {
				sceneCount = 11;
			}
			//scene12
			if (-variationX >= 32 * 57 && -variationX < 32 * 60) {
				sceneCount = 12;
			}

			//scene13 �����X�^�[������n�炸�ジ���肵����

			//scene14
			if (-variationX >= 32 * 73 && -variationX < 32 * 75) {
				sceneCount = 14;
			}

			//scene15
			if (-variationX >= 32 * 75 && -variationX < 32 * 82 && sceneCount == 14) {
				sceneCount = 15;
			}
			//scene16
			if (-variationX >= 32 * 82 && -variationX < 32 * 97) {
				sceneCount = 16;
			}

			//scene17
			if (-variationX >= 32 * 97 && -variationX < 32 * 104 && sceneCount == 16) {
				sceneCount = 17;
			}
			//scene18
			if (-variationX >= 32 * 104 && -variationX < 32 * 106) {
				sceneCount = 18;
			}

			//scene19
			if (-variationX >= 32 * 106 && -variationX < 32 * 113 && sceneCount == 18) {
				sceneCount = 19;
			}
			//scene20
			if (-variationX >= 32 * 113 && -variationX < 32 * 141) {
				sceneCount = 20;
			}

			//scene21
			if (-variationX >= 32 * 141 && -variationX < 32 * 149 && sceneCount == 20) {
				sceneCount = 21;
			}
			//scene22
			if (-variationX >= 32 * 149 && -variationX < 32 * 154) {
				sceneCount = 22;
			}

			//scene23
			if (-variationX >= 32 * 154 && -variationX < 32 * 162 && sceneCount == 22) {
				sceneCount = 23;
			}
			//scene24
			if (-variationX >= 32 * 162 && -variationX < 32 * 169 && sceneCount == 23) {
				sceneCount = 24;
			}

			//scene25 �ʏ�̏������~�߂ĕ�����`��
			if (-variationX >= 32 * 169 && sceneCount == 24) {
				callFlag = false;
				sceneCount = 25;
			}

			//�V�[������
			//��V�[���������當����`��
			if (sceneCount % 2 == 1) {
				Scene(&count, &sceneCount);
			}
			//�����V�[����������J�E���^��������
			else if (sceneCount % 2 == 0 && count != 255) {
				count = 255;
			}


			//FPS��\��
			//Draw();

			//����ʂ�\��ʂɔ��f
			ScreenFlip();

			//FPS�����킹�邽�ߑҋ@
			Wait();
		}

		//sceneCount��0�����Ȃ�v�����[�O��\��
		else {
			//�v�����[�O
			ClearDrawScreen();
			sceneCount = 0;
			while (count <= 255) {
				ProcessMessage();


				//���͂�`��@���X�ɕ������Z���Ȃ��Ă������o
				if (sceneCount == 0) {
					DrawFormatString(32 * 3, 32 * 2, GetColor(count, count, count), "���������ƑO���炱���ɂ����C������");
				}
				if (sceneCount == 1) {
					DrawFormatString(32 * 3, 32 * 3, GetColor(count, count, count), "�܂��͐[�����ɕ�܂�Ă���");
				}
				if (sceneCount == 2) {
					DrawFormatString(32 * 3, 32 * 4, GetColor(count, count, count), "�ǂ����������珗�̐l�̐�������");
				}
				if (sceneCount == 3) {
					DrawFormatString(32 * 3, 32 * 5, GetColor(count, count, count), "�l�͓������悤�ɐ��̂���ق��ւƕ����čs����");
				}
				if (sceneCount == 4) {
					DrawFormatString(32 * 3, 32 * 6, GetColor(count, count, count), "���΂炭����ƍ��Ɍq����@�ڂ�z���܂Ƃ������̐l�����ꂽ");
				}
				if (sceneCount == 5) {
					DrawFormatString(32 * 3, 32 * 7, GetColor(count, count, count), "���̐l�͖l�ɋ~�������߂Ă���悤������");
				}
				if (sceneCount == 6) {
					DrawFormatString(32 * 3, 32 * 8, GetColor(count, count, count), "�l�ɂ́@�����ł��Ȃ�����");
				}
				if (sceneCount == 7) {
					DrawFormatString(32 * 3, 32 * 9, GetColor(count, count, count), "���͎���ɔZ���Ȃ�@�����ĉ��������Ȃ��Ȃ���");
				}
				if (sceneCount == 8) {
					DrawFormatString(32 * 3, 32 * 10, GetColor(count, count, count), ".......................................");
				}
				if (sceneCount == 9) {
					DrawFormatString(32 * 3, 32 * 11, GetColor(count, count, count), "�C���t���ƌ��m��ʏꏊ�ɓ|��Ă���");
				}
				if (sceneCount == 10) {
					DrawFormatString(32 * 3, 32 * 12, GetColor(count, count, count), "�l�͋߂��ɂ����������܂���ɂ��Ă�������Ƃ炵��");
				}
				if (sceneCount == 11) {
					DrawFormatString(32 * 17, 32 * 13, GetColor(count, count, count), "\"��\"");
				}
				if (count == 255) {
					count = 0;
					if (sceneCount < 12) {
						sceneCount++;
					}
				}
				//�Ō�܂ŕ\�����ꂽ�灨�L�[���������ƂŃv�����[�O�I��
				if (sceneCount >= 11) {
					if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
						break;
					}
				}
				count++;

				ScreenFlip();
			}
			//�v�����[�O���I��������ϐ���������
			count = 255;
			sceneCount = 0;
		}
	}

	DxLib_End();

	return 0;
}

//��ʂ̗h������֐�
int quake(int quakeY) {
	if (quakeY == 0) {
		return 5;
	}
	else if (quakeY > 0) {
		return -1 * quakeY;
	}
	else if (quakeY < 0) {
		return -1 * quakeY - 1;
	}
}

//�V�[������
//��V�[���͕�����`�� �����͏����������Ȃ��Ă��� �����V�[���͊�V�[���̊ԂȂ̂łȂɂ��\�������ɃJ�E���^��������
void Scene(int *count, int *sceneCount) {
	if (*sceneCount == 1) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "��..������..�����́H...�Â��ăW���W�����Ă�...\n�Ȃ�ł���ȂƂ����... ��������E�o���Ȃ����\n\"��\"��\"��\"�ɏo���͂Ȃ�����");
	}

	if (*sceneCount == 3) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "���������Ă���...\n���ɗ���������Ȃ�\n�E���Ă����Ă�����");
	}

	if (*sceneCount == 5) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "\"��\"�ɏオ��������������֍s���邩��");
	}

	if (*sceneCount == 7) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "�I�H\n�Ȃ񂾂�����\n�����ƍs���΁@�����Ƒ��v");
	}

	if (*sceneCount == 9) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "���[��@�z�����Ȃ���\n�������̂����Ȃ�󂹂邩��\n\"C\" �Ăъ񂹂�");
	}

	if (*sceneCount == 11) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "�m�������Ƃ��납�痎���ċC����������");
	}

	if (*sceneCount == 13) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "�����|���̂����H\n����n��Ȃ��݂�����\n�ǂ��ɂ����Č��𖄂߂Ȃ���\n\"X\" �y���^��Ō��𖄂߂�");
	}

	if (*sceneCount == 15) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "�Ȃɂ������邽�߂ɂ����ɗ����͂�...\n�ł��Ȃɂ����ɂ����񂾁H");
	}

	if (*sceneCount == 17) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "��������̐l�ɑ���o�����\n�����ɂ���Ă���...\n�����d�v�ȁ@�ʂ����Ȃ���΂Ȃ�Ȃ��g��");
	}

	if (*sceneCount == 19) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "�N���̂�...\n�����܂ŔR�₹�邩��\n\"X\"�@�w偂̑���R�₷");
	}

	if (*sceneCount == 21) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "�E������...\n����͖l�����Ƃ������́H");
	}

	if (*sceneCount == 23) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "���������Ă��ĉ������悤�Ƃ��Ă����H");
	}

	if (*sceneCount == 25) {
		*sceneCount = 0;
		//�ʏ�̏������~�߂ĕ�����`��
		while (1) {
			ProcessMessage();

			//���͂�`��@���X�ɕ����������Ȃ��Ă������o
			if (*sceneCount == 0) {
				DrawFormatString(32 * 3, 32 * 2, GetColor(*count, *count, *count), "�������@�v���o����");
			}
			if (*sceneCount == 1) {
				DrawFormatString(32 * 3, 32 * 3, GetColor(*count, *count, *count), "�l�͉�����|�����߂ɑ��݂̂�Ȃɑ���o���ꂽ��");
			}
			if (*sceneCount == 2) {
				DrawFormatString(32 * 3, 32 * 4, GetColor(*count, *count, *count), "���͉������h�����߂ɓn���ꂽ");
			}
			if (*sceneCount == 3) {
				DrawFormatString(32 * 3, 32 * 5, GetColor(*count, *count, *count), "���ꂪ�l�̎g���@�ł��ǂ������...");
			}
			if (*sceneCount == 4) {
				DrawFormatString(32 * 3, 32 * 6, GetColor(*count, *count, *count), "\"X\"�@����U��");
			}

			if (*count == 0) {
				*count = 255;
				if (*sceneCount < 5) {
					*sceneCount += 1;
				}
			}
			//�Ō�܂ŕ\�����ꂽ��scene25�I��
			if (*sceneCount >= 5) {
				break;
			}
			*count += -1;

			ScreenFlip();
		}
		*sceneCount = 26;
	}

	*count += -1;

	//�J�E���^���Ō�܂ōs������V�[���͎���
	if (*count == 0) {
		*sceneCount += 1;
	}
}