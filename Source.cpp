#include <math.h>
#include <DxLib.h>

//フレームレートを変えるためのパラメータ
static int mStartTime; //測定開始時刻
static int mCount; //カウンタ
static float mFps; //fps
static const int N = 30; //平均をとるサンプル数
static const int FPS = 30; //設定したFPS

//関数の定義
bool Update() {
	//1フレーム目なら時刻を記憶
	if (mCount == 0) {
		mStartTime = GetNowCount();
	}
	//60フレーム目なら平均を計算する
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
	//かかった時間
	int tookTime = GetNowCount() - mStartTime;
	//待つべき時間
	int waitTime = mCount * 1000 / FPS - tookTime;
	if (waitTime > 0) {
		//待機
		Sleep(waitTime);
	}
}

int quake(int);

void Scene(int*, int*);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	ChangeWindowMode(TRUE);
	DxLib_Init();
	//ロード表示
	DrawFormatString(240, 240, GetColor(255, 255, 255), "Now Loading...");

	//裏画面に描画指定
	SetDrawScreen(DX_SCREEN_BACK);



	//初期位置
	int playerX = 32 * 10 - 16, playerY = 32 * 11;
	int monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9
	
	//各種パラメータ設定
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

	//各種フラグ設定
	bool jumpFlag = false;
	bool callFlag = false;
	bool playerLeftDirection = true;
	bool playerRightDirection = false;
	bool monsterLeftDirection = true;
	bool monsterRightDirection = false;
	bool monsterAction = false;

	//マップデータ読み取り
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



	//キーの状態を格納する配列を用意
	char key[256];

	//画像を用意
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



	//ゲーム部分
	while (1) {
		//sceneCountが0以上でゲーム部分が始まる
		if (sceneCount >= 0) {
			ProcessMessage();
			Update();

			//全てのキーの入力を監視する
			GetHitKeyStateAll(key);

			//終了
			if (key[KEY_INPUT_Q] == 1) {
				break;
			}

			//画面の描画を消す
			ClearDrawScreen();



			//背景描画
			DrawGraph(0, 0, background, TRUE);



			//マップ生成
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

			//モブキャラ描画
			for (x = 0; x < 8; x++) {
				DrawGraph(32 * (199 - 2 * x) + variationX, 32 * 11 - 16, mob1, TRUE);
				DrawGraph(32 * (198 - 2 * x) + variationX, 32 * 11 - 16, mob2, TRUE);
			}




			//プレイヤー処理
			//マップスクロール
			//左キーが押されて、プレイヤーの左上と左下に何もなければ左に進む
			if (key[KEY_INPUT_LEFT] == 1) {
				if (map[(int)(playerY / 32)][(int)(playerX - 1 - variationX) / 32] == 0 && map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - 1 - variationX) / 32)] == 0) {
					variationX += 2;
				}
				playerLeftDirection = true;
				playerRightDirection = false;
				playerWalkCount += 1;
			}
			//右キーが押されて、プレイヤーの右上と右下に何もなければ右に進む
			if (key[KEY_INPUT_RIGHT] == 1) {
				if (map[(int)(playerY / 32)][(int)(playerX + playerWidth - variationX) / 32] == 0 && map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] == 0 && -variationX < 32 * 180) {
					variationX += -2;
				}
				playerRightDirection = true;
				playerLeftDirection = false;
				playerWalkCount += 1;
			}

			//ジャンプ処理
			//ジャンプ中は下方向に加速
			if (jumpFlag == true) {
				playerY += acceleY + acceleG;
				acceleG += 1;
			}
			//プレイヤーが床についたらフラグを戻し、床へのめり込みを戻す
			if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY + playerHeight - 1) / 32)][(int)(playerX + playerWidth - 1 - variationX) / 32] != 0) {
				jumpFlag = false;
				while (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] != 0) {
					playerY += -1;
				}
			}
			//プレイヤーが床にいる時に上キーが押されたら、フラグを立て加速度を設定
			if (key[KEY_INPUT_UP] == 1 && jumpFlag == false) {
				jumpFlag = true;
				acceleY = -11;
				acceleG = 0;
			}
			//上キーを短く押すと小ジャンプ
			if (jumpFlag == true && key[KEY_INPUT_UP] == 0) {
				acceleG += 1;
			}
			//足場から落ちたとき
			if (jumpFlag == false && map[(int)((playerY + playerHeight) / 32)][(int)((playerX - variationX) / 32)] == 0 && map[(int)((playerY + playerHeight) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] == 0) {
				jumpFlag = true;
				acceleY = 0;
				acceleG = 0;
			}
			//プレイヤーが天井に着いたら、天井へのめり込みを戻す
			if (map[(int)((playerY - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY - 1) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] != 0) {
				while (map[(int)((playerY - 1) / 32)][(int)((playerX - variationX) / 32)] != 0 || map[(int)((playerY - 1) / 32)][(int)((playerX + playerWidth - 1 - variationX) / 32)] != 0) {
					playerY += 1;
				}
			}

			//プレイヤーアクション処理
			if (key[KEY_INPUT_X] == 1) {
				//土を運ぶ
				//プレイヤーの右側の土を運ぶ
				if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] == 4) {
					map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] = 0;
					for (x = 0; x < mapWidth; x++) {
						if (map[12][x] == 0) {
							map[12][x] = 2;
							break;
						}
					}
				}
				//プレイヤーの左側の土を運ぶ
				else if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX - 1) / 32)] == 4) {
					map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX - 1) / 32)] = 0;
					for (x = 0; x < mapWidth; x++) {
						if (map[12][x] == 0) {
							map[12][x] = 2;
							break;
						}
					}
				}

				//蜘蛛の巣を燃やす
				//プレイヤーの右側の蜘蛛の巣を燃やす
				if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX + playerWidth - variationX) / 32)] == 5) {
					fireCount = 0;
				}
				//プレイヤーの左側の蜘蛛の巣を燃やす
				else if (map[(int)((playerY + playerHeight - 1) / 32)][(int)((playerX - variationX - 1) / 32)] == 5) {
					fireCount = 0;
				}

				//モンスターを剣で刺す
				if (sceneCount == 26 && ((monsterX + variationX <= playerX && monsterX + monsterWidth + variationX > playerX) || (monsterX + variationX <= playerX + playerWidth && monsterX + monsterWidth + variationX > playerX + playerWidth))) {
					count = 0;
					sceneCount = 0;
					ClearDrawScreen();
					//エンディング
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

							//リスタート
							if (CheckHitKey(KEY_INPUT_R) == 1) {
								//変数を初期化
								//初期位置
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

								//各種パラメータ設定
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = -1;

								//各種フラグ設定
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//マップ初期化
								for (y = 0; y < mapHeight; y++) {
									for (x = 0; x < mapWidth; x++) {
										map[y][x] = syokika[y][x];
									}
								}

								break;
							}

							//プロローグとばしてリスタート
							if (CheckHitKey(KEY_INPUT_P) == 1) {
								//変数を初期化
								//初期位置
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

																	   //各種パラメータ設定
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = 0;

								//各種フラグ設定
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//マップ初期化
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

				//村人を剣で刺す
				if (sceneCount == 26 && -variationX >= 32 * 170) {
					count = 0;
					sceneCount = 0;
					ClearDrawScreen();
					//エンディング
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

							//リスタート
							if (CheckHitKey(KEY_INPUT_R) == 1) {
								//変数を初期化
								//初期位置
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

								//各種パラメータ設定
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = -1;

								//各種フラグ設定
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//マップ初期化
								for (y = 0; y < mapHeight; y++) {
									for (x = 0; x < mapWidth; x++) {
										map[y][x] = syokika[y][x];
									}
								}


								break;
							}

							//リスタート
							if (CheckHitKey(KEY_INPUT_P) == 1) {
								//変数を初期化
								//初期位置
								playerX = 32 * 10 - 16, playerY = 32 * 11;
								monsterX = 32 * 43, monsterY = 32 * 9; // monsterX = 32 * 43, monsterY = 32 * 9

																	   //各種パラメータ設定
								variationX = -32 * 2; // variationX = -32 * 2
								acceleY = 0, acceleG = 0;
								playerWalkCount = 0;
								monsterActionCount = 0;
								fireCount = -1;
								quakeY = 0;
								count = 255;
								sceneCount = 0;

								//各種フラグ設定
								jumpFlag = false;
								callFlag = false;
								playerLeftDirection = true;
								playerRightDirection = false;
								monsterLeftDirection = true;
								monsterRightDirection = false;
								monsterAction = false;

								//マップ初期化
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

			//蜘蛛の巣が燃えるアニメーション
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

			//剣描画 scene03を過ぎたら非表示
			if (sceneCount < 4) {
				DrawGraph(32 * 22 + variationX, 32 * 11, sword, TRUE);
			}



			//モンスター処理
			//モンスター呼び出しアクション
			if ((monsterX + variationX > playerX - 10 && monsterX + variationX < playerX - 6) || monsterX > 32 * 177) {
				callFlag = false;
			}
			if (key[KEY_INPUT_C] == 1 && variationX <= -32 * 29 && variationX > -32 * 168) {
				callFlag = true;
			}


			//呼び出し処理
			if (callFlag == true) {
				//モンスターがプレイヤーより左にいれば右に歩く
				if (monsterX + variationX < playerX - 6) {
					//右に壁がなければ右に歩く
					if (map[(int)(monsterY / 32)][(int)((monsterX + monsterWidth) / 32)] == 0 && map[(int)((monsterY + monsterHeight - 1) / 32)][(int)((monsterX + monsterWidth) / 32)] == 0) {
						monsterX += 3;
						monsterRightDirection = true;
						monsterLeftDirection = false;
					}
					//右に壊せる岩があれば
					else if (map[(int)((monsterY + monsterHeight - 1) / 32)][(int)((monsterX + monsterWidth) / 32)] == 3) {
						monsterAction = true;
					}
					//穴があると渡らずに後ずさり
					if (map[(int)((monsterY + monsterHeight) / 32)][(int)((monsterX + monsterWidth) / 32)] == 0) {
						monsterX += -9;
						callFlag = false;
						//scene13を描画
						sceneCount = 13;
					}
				}
				//モンスターがプレイヤーより右にいれば左に歩く
				else if (monsterX + variationX > playerX - 10) {
					//左に壁がなければ左に歩く
					if (map[(int)(monsterY / 32)][(int)((monsterX - 1) / 32)] == 0 && map[(int)((monsterY + monsterHeight - 1) / 32)][(int)((monsterX - 1) / 32)] == 0) {
						monsterX += -3;
						monsterLeftDirection = true;
						monsterRightDirection = false;
					}
				}
			}

			//モンスター描画
			//右向きに描画
			if (monsterRightDirection == true) {
				//岩を壊すアニメーション
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
							//モンスターの隣の岩を消す
							for (int i = monsterHeight; i > 0; i += -32) {
								map[(int)((monsterY + i - 1) / 32)][(int)((monsterX + monsterReachX) / 32)] = 0;
							}
						}
					}
					monsterActionCount += 1;
				}
				//歩く,立つ
				else {
					DrawGraph(monsterX + variationX, monsterY, monsterRightStand, TRUE);
				}
			}
			//左向きに描画
			else if (monsterLeftDirection == true) {
				DrawGraph(monsterX + variationX, monsterY, monsterLeftStand, TRUE);
			}



			//プレイヤー描画
			//左向きに歩く
			if (playerLeftDirection == true) {
				//左キーが押されたら右向きに歩くアニメーション
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
				//左キーが押されてなかったら左向きに立つ静止画
				else if (key[KEY_INPUT_LEFT] == 0) {
					DrawGraph(playerX, playerY, playerLeftStand, TRUE);
					playerWalkCount = -1;
				}
			}
			//右向きに歩く
			else if (playerRightDirection == true) {
				//右キーが押されたら右向きに歩くアニメーション
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
				//右キーが押されてなかったら右向きに立つ静止画
				else if (key[KEY_INPUT_RIGHT] == 0) {
					DrawGraph(playerX, playerY, playerRightStand, TRUE);
					playerWalkCount = -1;
				}
			}


			//暗闇を描画
			if (-variationX < 32 * 156) {
				DrawGraph(0, playerY - 32 * 12, mask1, TRUE);
			}
			if (-variationX >= 32 * 156 && -variationX < 32 * 167) {
				DrawGraph(0, playerY - 32 * 12, mask2, TRUE);
			}


			//シーン管理
			//scene01
			//シーンの場所に入ったら描画
			if (-variationX >= 0 && -variationX < 32 * 8 && sceneCount == 0) {
				sceneCount = 1;
			}
			//scene02
			//シーンの場所から出たら永久に描画しない
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

			//scene13 モンスターが穴を渡らず後ずさりした時

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

			//scene25 通常の処理を止めて文字を描画
			if (-variationX >= 32 * 169 && sceneCount == 24) {
				callFlag = false;
				sceneCount = 25;
			}

			//シーン処理
			//奇数シーンだったら文字を描画
			if (sceneCount % 2 == 1) {
				Scene(&count, &sceneCount);
			}
			//偶数シーンだったらカウンタを初期化
			else if (sceneCount % 2 == 0 && count != 255) {
				count = 255;
			}


			//FPSを表示
			//Draw();

			//裏画面を表画面に反映
			ScreenFlip();

			//FPSを合わせるため待機
			Wait();
		}

		//sceneCountが0未満ならプロローグを表示
		else {
			//プロローグ
			ClearDrawScreen();
			sceneCount = 0;
			while (count <= 255) {
				ProcessMessage();


				//文章を描画　徐々に文字が濃くなっていく演出
				if (sceneCount == 0) {
					DrawFormatString(32 * 3, 32 * 2, GetColor(count, count, count), "もうずっと前からここにいた気がする");
				}
				if (sceneCount == 1) {
					DrawFormatString(32 * 3, 32 * 3, GetColor(count, count, count), "まわりは深い霧に包まれていた");
				}
				if (sceneCount == 2) {
					DrawFormatString(32 * 3, 32 * 4, GetColor(count, count, count), "どこか遠くから女の人の声がする");
				}
				if (sceneCount == 3) {
					DrawFormatString(32 * 3, 32 * 5, GetColor(count, count, count), "僕は導かれるように声のするほうへと歩いて行った");
				}
				if (sceneCount == 4) {
					DrawFormatString(32 * 3, 32 * 6, GetColor(count, count, count), "しばらくすると鎖に繋がれ　ぼろ布をまとった女の人が現れた");
				}
				if (sceneCount == 5) {
					DrawFormatString(32 * 3, 32 * 7, GetColor(count, count, count), "その人は僕に救いを求めているようだった");
				}
				if (sceneCount == 6) {
					DrawFormatString(32 * 3, 32 * 8, GetColor(count, count, count), "僕には　何もできなかった");
				}
				if (sceneCount == 7) {
					DrawFormatString(32 * 3, 32 * 9, GetColor(count, count, count), "霧は次第に濃くなり　そして何も見えなくなった");
				}
				if (sceneCount == 8) {
					DrawFormatString(32 * 3, 32 * 10, GetColor(count, count, count), ".......................................");
				}
				if (sceneCount == 9) {
					DrawFormatString(32 * 3, 32 * 11, GetColor(count, count, count), "気が付くと見知らぬ場所に倒れていた");
				}
				if (sceneCount == 10) {
					DrawFormatString(32 * 3, 32 * 12, GetColor(count, count, count), "僕は近くにあったたいまつを手にしてあたりを照らした");
				}
				if (sceneCount == 11) {
					DrawFormatString(32 * 17, 32 * 13, GetColor(count, count, count), "\"→\"");
				}
				if (count == 255) {
					count = 0;
					if (sceneCount < 12) {
						sceneCount++;
					}
				}
				//最後まで表示されたら→キーを押すことでプロローグ終了
				if (sceneCount >= 11) {
					if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
						break;
					}
				}
				count++;

				ScreenFlip();
			}
			//プロローグが終了したら変数を初期化
			count = 255;
			sceneCount = 0;
		}
	}

	DxLib_End();

	return 0;
}

//画面の揺れを作る関数
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

//シーン処理
//奇数シーンは文字を描画 文字は少しずつ薄くなっていく 偶数シーンは奇数シーンの間なのでなにも表示せずにカウンタを初期化
void Scene(int *count, int *sceneCount) {
	if (*sceneCount == 1) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "う..ううん..ここは？...暗くてジメジメしてる...\nなんでこんなところに... ここから脱出しなければ\n\"←\"や\"→\"に出口はないかな");
	}

	if (*sceneCount == 3) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "剣が落ちている...\n役に立つかもしれない\n拾ってもっていこう");
	}

	if (*sceneCount == 5) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "\"↑\"に上がったら向こう側へ行けるかも");
	}

	if (*sceneCount == 7) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "！？\nなんだあいつは\nそっと行けば　きっと大丈夫");
	}

	if (*sceneCount == 9) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "うーん　越えられないな\nさっきのあいつなら壊せるかも\n\"C\" 呼び寄せる");
	}

	if (*sceneCount == 11) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "確か高いところから落ちて気を失ったんだ");
	}

	if (*sceneCount == 13) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "穴が怖いのかい？\n道を渡れないみたいだ\nどうにかして穴を埋めないと\n\"X\" 土を運んで穴を埋める");
	}

	if (*sceneCount == 15) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "なにかをするためにここに来たはず...\nでもなにをしにきたんだ？");
	}

	if (*sceneCount == 17) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "たくさんの人に送り出されて\nここにやってきた...\n何か重要な　果たさなければならない使命");
	}

	if (*sceneCount == 19) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "クモのす...\nたいまつで燃やせるかな\n\"X\"　蜘蛛の巣を燃やす");
	}

	if (*sceneCount == 21) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "拾った剣...\nあれは僕が落としたもの？");
	}

	if (*sceneCount == 23) {
		DrawFormatString(32 * 5, 32 * 2, GetColor(*count, *count, *count), "剣を持ってきて何をしようとしていた？");
	}

	if (*sceneCount == 25) {
		*sceneCount = 0;
		//通常の処理を止めて文字を描画
		while (1) {
			ProcessMessage();

			//文章を描画　徐々に文字が薄くなっていく演出
			if (*sceneCount == 0) {
				DrawFormatString(32 * 3, 32 * 2, GetColor(*count, *count, *count), "そうだ　思い出した");
			}
			if (*sceneCount == 1) {
				DrawFormatString(32 * 3, 32 * 3, GetColor(*count, *count, *count), "僕は怪物を倒すために村のみんなに送り出されたんだ");
			}
			if (*sceneCount == 2) {
				DrawFormatString(32 * 3, 32 * 4, GetColor(*count, *count, *count), "剣は怪物を刺すために渡された");
			}
			if (*sceneCount == 3) {
				DrawFormatString(32 * 3, 32 * 5, GetColor(*count, *count, *count), "それが僕の使命　でもどうすれば...");
			}
			if (*sceneCount == 4) {
				DrawFormatString(32 * 3, 32 * 6, GetColor(*count, *count, *count), "\"X\"　剣を振る");
			}

			if (*count == 0) {
				*count = 255;
				if (*sceneCount < 5) {
					*sceneCount += 1;
				}
			}
			//最後まで表示されたらscene25終了
			if (*sceneCount >= 5) {
				break;
			}
			*count += -1;

			ScreenFlip();
		}
		*sceneCount = 26;
	}

	*count += -1;

	//カウンタが最後まで行ったらシーンは次へ
	if (*count == 0) {
		*sceneCount += 1;
	}
}