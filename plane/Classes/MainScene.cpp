#include"MainScene.h"
#include "GameData.h"
bool MainScene::playEffect = true;
bool MainScene::playbgtmusic = true;
bool MainScene::init()
{
	if(!Layer::init())
	{
		return false;
	}
	buttetFlag = true;
	bigBoomCount = 0;
	playerScore  = 0;

	GameScene * gameLayer = GameScene::create();
	this->addChild(gameLayer);

	controlLayer = ControlLayer::create();
	this->addChild(controlLayer,100);

	planeLayer = PlaneLayer::create();
	this->addChild(planeLayer);

	bulletLayer = BulletLayer::create();
	this->addChild(bulletLayer);

	enemyLayer = EnemyLayer::create();
	this->addChild(enemyLayer);

	mutibullets=MutiBullets::create();
	this->addChild(mutibullets);

	ufoLayer = UFOLayer::create();
	this->addChild(ufoLayer);

	this->GameStart();

	GameData::Inst()->resetSpeed();

	//设置触摸事件监听
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	return true;
}
Scene* MainScene::scene()
{
	Scene* scene = NULL;
	scene  = Scene::create();
	MainScene* layer = MainScene::create();
	scene->addChild(layer);
	return scene;
}
bool MainScene::onTouchBegan(Touch *pTouch,Event *pEvent)
{
	return true;
}
void MainScene::GameStart()
{
	this->scheduleUpdate();
	bulletLayer->StartShoot(0.1f);
	this->schedule(schedule_selector(MainScene::produceGameItem),0.5f);
	this->schedule(schedule_selector(MainScene::isGameOver),8.0f);
	this->schedule(schedule_selector(MainScene::produceGameItem2),10.0f);
}
void MainScene::GameStop()
{
	unscheduleUpdate();
	bulletLayer->StopShoot();
	this->unschedule(schedule_selector(MainScene::produceGameItem));
	this->unschedule(schedule_selector(MainScene::produceGameItem2));
	this->unschedule(schedule_selector(MainScene::isGameOver));

}
void MainScene::isGameOver(float dt)
{
	if(this->planeLayer->isAlive && !Director::getInstance()->isPaused())
	{
		enemyLayer->addEnemy1(1);
	}else
	{
		//提示是否复活
		//this->planeLayer->isAlive=true;
		//planeLayer = PlaneLayer::create();
		//GameStop();
		GameOver();
	}
}
void MainScene::produceGameItem(float dt)
{
	if(this->planeLayer->isAlive && !Director::getInstance()->isPaused())
	{
		int max=1000,min=1;
		int flag = rand()%(max+1-min)+min;
		if (flag>0 && flag <=700)
		{
			enemyLayer->addEnemy1(1);
		}
		else if (flag>700 && flag <=900)
		{
			enemyLayer->addEnemy1(1);
		}
		else if (flag>900 && flag <=950)
		{
			enemyLayer->addEnemy1(1);
		}
		else
		{
			enemyLayer->addEnemy1(1);
		}
	}
}
void MainScene::produceGameItem2(float dt)
{
	if(this->planeLayer->isAlive && !Director::getInstance()->isPaused())
	{
		int max=100,min=1;
		int flag = rand()%(max+1-min)+min;
		if (flag>0 && flag <=50)
		{
			enemyLayer->addEnemy3(1);
		}
		else if (flag>60 && flag <=80)
		{
			ufoLayer->AddBigBoomItem(0.1f);
		}
		else
		{
			ufoLayer->AddMutiBullets(0.1f);
		}
	}
}
void MainScene::onTouchMoved(Touch *pTouch,Event *pEvent)
{
	if(this->planeLayer->isAlive && !Director::getInstance()->isPaused())
	{

		Point beginPoint = pTouch->getLocationInView();
		beginPoint = Director::getInstance()->convertToGL(beginPoint);

		Rect planeRect = this->planeLayer->getChildByTag(1)->boundingBox();
		planeRect.origin.x-=15;
		planeRect.origin.y-=15;
		planeRect.size.width+=30;
		planeRect.size.height+=30;
		if(planeRect.containsPoint(beginPoint))
		{
			Point endPoint = pTouch->getPreviousLocationInView();
			endPoint = Director::getInstance()->convertToGL(endPoint);
			Point offset = beginPoint - endPoint; 
			Point toPoint = this->planeLayer->getChildByTag(1)->getPosition() + offset;
			this->planeLayer->MoveTo(toPoint);
		}

	}
}

void MainScene::update(float dt)
{
	//playerAndEnemy();
	if (this->planeLayer->isAlive)
	{
		if (this->bulletLayer->m_pAllBullet != NULL)
		{
			SingleBulletCrash();
		}

		if (this->mutibullets->m_pAllBullet != NULL)
		{
			MutiBulletCrash();
		}

		planeAndUFOCrash();
		planeAndUFOBoomCrash();

		playerAndEnemy();
		playerAndEnemy3();
	}

}
void MainScene::SingleBulletCrash()
{
	isBullet2AndEnemy();
	isBulletAndEnemy();
	isBullet3AndEnemy();
}
void MainScene::MutiBulletCrash()
{
	MutibulletAndEnemy();
	Mutibullet2AndEnemy();
	Mutibullet3AndEnemy();
}
void MainScene::playerAndEnemy()
{
	auto enemy1ToDelete = __Array::create();
	enemy1ToDelete->retain();
	Ref* et;

	CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy1,et)
	{
		Enemy* enemy1 = (Enemy*)et;
		if (enemy1->getBoundingBox().intersectsRect(this->planeLayer->getBoundingBox()))
		{
			enemy1ToDelete->addObject(enemy1);
			this->bulletLayer->StopShoot();
			this->mutibullets->StopShoot();
			this->unschedule(schedule_selector(MainScene::produceGameItem));
			this->unschedule(schedule_selector(MainScene::produceGameItem2));

			this->planeLayer->PlayerBlowup(PlaneLayer::sharedPlane->getPlayerPlane());
			// 弹出十分复活界面
			//CCLOG("plane is bad");
			if (GameData::Inst()->isRelive()==1) {
				planeLayer = PlaneLayer::create();
				this->addChild(planeLayer);
				bulletLayer->StartShoot(0.1f);
				this->schedule(schedule_selector(MainScene::produceGameItem),0.5f);
				this->schedule(schedule_selector(MainScene::produceGameItem2),10.0f);
				this->planeLayer->isAlive=true;
			}else
			{
				this->planeLayer->isAlive=false;
			}
			break;
		}
	}
	CCARRAY_FOREACH(enemy1ToDelete,et)
	{
		Enemy* enemy1=(Enemy*)et;
		this->enemyLayer->enemy1Blowup(enemy1);
	}
	enemy1ToDelete->release();

}
void MainScene::playerAndEnemy3()
{
	__Array* enemy1ToDelete = __Array::create();
	enemy1ToDelete->retain();
	Ref* et;
	CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy3,et)
	{
		Enemy* enemy1 = (Enemy*)et;

		if (enemy1->getBoundingBox().intersectsRect(this->planeLayer->getBoundingBox()))
		{

			enemy1ToDelete->addObject(enemy1);
			this->planeLayer->isAlive=false;
			this->bulletLayer->StopShoot();
			this->mutibullets->StopShoot();
			this->enemyLayer->removeAllEnemyNow1();
			this->enemyLayer->removeAllEnemyNow3();

			this->planeLayer->PlayerBlowup(PlaneLayer::sharedPlane->getPlayerPlane());
			//是否复活
			if (GameData::Inst()->isRelive()==1) {
				planeLayer = PlaneLayer::create();
				this->addChild(planeLayer);
				bulletLayer->StartShoot();
				this->planeLayer->isAlive=true;
			}else
			{
				this->planeLayer->isAlive=false;
			}
			break;
			//this->removeChild(this->planeLayer);
		}
	}

	CCARRAY_FOREACH(enemy1ToDelete,et)
	{
		Enemy* enemy1=(Enemy*)et;
		this->enemyLayer->enemy3Blowup(enemy1);
	}
	enemy1ToDelete->release();


}
void MainScene::GameOver()
{
	if (GameData::Inst()->isEffectOpen) {

		SimpleAudioEngine::getInstance()->playEffect("sound/game_over.mp3",false);
	}
	//	Scene *scene = GameoverLayer::scene(this->playerScore);
	//	Director::getInstance()->replaceScene(scene);
	//弹出复活界面
	GameStop();

	//    relive = reliveLayer::create(this,SEL_CallFuncO(&MainScene::cbRelive));
	//    relive->setScale(0.1);
	//    auto scaleOne=ScaleTo::create(0.1f, 0.3f);
	//    auto scaleTwo=ScaleTo::create(0.1f, 0.6f);
	//    auto scaleThr=ScaleTo::create(0.1f, 0.8f);
	//    auto scaleFor=ScaleTo::create(0.1f, 1.0f);
	//    relive->runAction(Sequence::create(scaleOne,scaleTwo,scaleThr,scaleFor, NULL));
	//    addChild(relive,100);
	//	  relive->release();
	GameoverLayer *m_gameover=GameoverLayer::create(playerScore);
	m_gameover->setScale(0.1);
	auto scaleOne=ScaleTo::create(0.1f, 0.3f);
	auto scaleTwo=ScaleTo::create(0.1f, 0.6f);
	auto scaleThr=ScaleTo::create(0.1f, 0.8f);
	auto scaleFor=ScaleTo::create(0.1f, 1.0f);
	m_gameover->runAction(Sequence::create(scaleOne,scaleTwo,scaleThr,scaleFor, NULL));
	addChild(m_gameover,100);

}
void MainScene::cbRelive(Ref * pObj)
{
	reliveLayerRet* pRet = (reliveLayerRet*)pObj;
	log("chose = %d",pRet->iSel);

	if (pRet->iSel == 1) {
		relive=NULL;
		//this->removeChild(relive);
		GameoverLayer *m_gameover=GameoverLayer::create(playerScore);
		m_gameover->setScale(0.1);
		auto scaleOne=ScaleTo::create(0.1f, 0.3f);
		auto scaleTwo=ScaleTo::create(0.1f, 0.6f);
		auto scaleThr=ScaleTo::create(0.1f, 0.8f);
		auto scaleFor=ScaleTo::create(0.1f, 1.0f);
		m_gameover->runAction(Sequence::create(scaleOne,scaleTwo,scaleThr,scaleFor, NULL));
		addChild(m_gameover,100);
	}else
	{
		relive=NULL;
		// GameData::Inst()->setJfNum(2000);
		if(GameData::Inst()->getJfNum()-1000 >= 0)
		{
			//计费返回成功
			//复活
			planeLayer = PlaneLayer::create();
			this->addChild(planeLayer);
			bulletLayer->StartShoot(0.1f);
			GameStart();
			this->planeLayer->isAlive=true;
			GameData::Inst()->setJfNum(-1000);
		}else
		{
			//积分不够弹窗口
			//GameData::Inst()->shwoWall();
		}
		//复活
		//        planeLayer = PlaneLayer::create();
		//        this->addChild(planeLayer);
		//        bulletLayer->StartShoot(0.1f);
		//        GameStart();
		//        this->planeLayer->isAlive=true;
		//        this->scheduleUpdate();
		//计费返回失败
		//        if (pRet->iSel != 2) {
		//            GameoverLayer *m_gameover=GameoverLayer::create(playerScore);
		//            m_gameover->setScale(0.1);
		//            auto scaleOne=ScaleTo::create(0.1f, 0.3f);
		//            auto scaleTwo=ScaleTo::create(0.1f, 0.6f);
		//            auto scaleThr=ScaleTo::create(0.1f, 0.8f);
		//            auto scaleFor=ScaleTo::create(0.1f, 1.0f);
		//            m_gameover->runAction(Sequence::create(scaleOne,scaleTwo,scaleThr,scaleFor, NULL));
		//            addChild(m_gameover,100);
		//        }else
		//        {
		//            //计费返回成功
		//            //复活
		//            planeLayer = PlaneLayer::create();
		//            this->addChild(planeLayer);
		//            bulletLayer->StartShoot(0.1f);
		//            GameStart();
		//            this->planeLayer->isAlive=true;
		//        }
	}
}
void MainScene::isBulletAndEnemy()
{
	__Array* bulletToDelete=__Array::create();
	bulletToDelete->retain();
	Ref *bt,*et;

	CCARRAY_FOREACH(this->bulletLayer->m_pAllBullet,bt)
	{
		Sprite *bullte =(Sprite*)bt;
		__Array* enemy1ToDelete = __Array::create();
		enemy1ToDelete->retain();
		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy1,et)
		{
			Enemy* enemy1 = (Enemy*)et;

			if (bullte->boundingBox().intersectsRect(enemy1->getBoundingBox()))
			{
				if (enemy1->getLife()>1)
				{
					enemy1->lostLife();
					bulletToDelete->addObject(bullte);
				}
				else if (enemy1->getLife()==1)
				{
					if (GameData::Inst()->isEffectOpen) {
						SimpleAudioEngine::getInstance()->playEffect("sound/enemy1_down.mp3",false);
					}
					enemy1->lostLife();
					bulletToDelete->addObject(bullte);
					enemy1ToDelete->addObject(enemy1);

					playerScore += DIJI1_SCORE;
					this->controlLayer->updateScore(playerScore);
				}
				break;
			}
		}

		CCARRAY_FOREACH(enemy1ToDelete,et)
		{
			Enemy* enemy1=(Enemy*)et;
			this->enemyLayer->enemy1Blowup(enemy1);
		}
		enemy1ToDelete->release();

	}
	CCARRAY_FOREACH(bulletToDelete,bt)
	{
		Sprite* bullet =(Sprite*)bt;
		this->bulletLayer->RemoveBullet(bullet);
	}
	bulletToDelete->removeAllObjects();
	bulletToDelete->release();
}
void MainScene::isBullet2AndEnemy()
{
	__Array* bulletToDelete2=__Array::create();
	bulletToDelete2->retain();
	Ref *bt,*et;
	CCARRAY_FOREACH(this->bulletLayer->m_pAllBullet,bt)
	{
		Sprite *bullte =(Sprite*)bt;
		__Array* enemy2ToDelete = __Array::create();
		enemy2ToDelete->retain();

		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy2,et)
		{
			Enemy* enemy2 = (Enemy*)et;

			if (bullte->boundingBox().intersectsRect(enemy2->getBoundingBox()))
			{
				if (enemy2->getLife()>1)
				{
					enemy2->lostLife();
					bulletToDelete2->addObject(bullte);
				}
				else if (enemy2->getLife()==1)
				{
					enemy2->lostLife();
					bulletToDelete2->addObject(bullte);
					enemy2ToDelete->addObject(enemy2);
					playerScore += DIJI2_SCORE;
					this->controlLayer->updateScore(playerScore);
				}
				break;
			}
		}

		CCARRAY_FOREACH(enemy2ToDelete,et)
		{
			Enemy* enemy2=(Enemy*)et;
			this->enemyLayer->enemy2Blowup(enemy2);
		}
		enemy2ToDelete->release();
	}
	CCARRAY_FOREACH(bulletToDelete2,bt)
	{
		Sprite* bullet =(Sprite*)bt;
		this->bulletLayer->RemoveBullet(bullet);
	}
	bulletToDelete2->removeAllObjects();
	bulletToDelete2->release();
}
void MainScene::isBullet3AndEnemy()
{
	__Array* bulletToDelete=__Array::create();
	bulletToDelete->retain();
	Ref *bt,*et;

	CCARRAY_FOREACH(this->bulletLayer->m_pAllBullet,bt)
	{
		Sprite *bullte =(Sprite*)bt;
		__Array* enemy3ToDelete = __Array::create();
		enemy3ToDelete->retain();

		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy3,et)
		{
			Enemy* enemy3 = (Enemy*)et;

			if (bullte->boundingBox().intersectsRect(enemy3->getBoundingBox()))
			{
				if (enemy3->getLife()>1)
				{
					enemy3->lostLife();
					bulletToDelete->addObject(bullte);
				}
				else if (enemy3->getLife()==1)
				{
					if (GameData::Inst()->isEffectOpen) {
						SimpleAudioEngine::getInstance()->playEffect("sound/enemy3_down.mp3",false);
					}
					enemy3->lostLife();
					bulletToDelete->addObject(bullte);
					enemy3ToDelete->addObject(enemy3);

					playerScore += DIJI3_SCORE;
					this->controlLayer->updateScore(playerScore);
				}
				break;
			}
		}

		CCARRAY_FOREACH(enemy3ToDelete,et)
		{
			Enemy* enemy3=(Enemy*)et;
			this->enemyLayer->enemy3Blowup(enemy3);
		}
		enemy3ToDelete->release();

	}
	CCARRAY_FOREACH(bulletToDelete,bt)
	{
		Sprite* bullet =(Sprite*)bt;
		this->bulletLayer->RemoveBullet(bullet);
	}
	bulletToDelete->removeAllObjects();
	bulletToDelete->release();
}

void MainScene::MutibulletAndEnemy()
{

	__Array* bulletToDelete=__Array::create();
	bulletToDelete->retain();
	Ref *bt,*et;

	CCARRAY_FOREACH(this->mutibullets->m_pAllBullet,bt)
	{
		Sprite *bullte =(Sprite*)bt;
		__Array* enemyToDelete = __Array::create();
		enemyToDelete->retain();
		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy1,et)
		{
			Enemy* enemy = (Enemy*)et;

			if (bullte->boundingBox().intersectsRect(enemy->getBoundingBox()))
			{
				if (enemy->getLife()>1)
				{
					enemy->lostLife();
					bulletToDelete->addObject(bullte);
				}
				else if (enemy->getLife()==1)
				{
					enemy->lostLife();
					bulletToDelete->addObject(bullte);
					enemyToDelete->addObject(enemy);

					playerScore += DIJI1_SCORE;
					this->controlLayer->updateScore(playerScore);
				}
				break;
			}
		}
		CCARRAY_FOREACH(enemyToDelete,et)
		{
			Enemy* enemy=(Enemy*)et;
			this->enemyLayer->enemy1Blowup(enemy);
		}
		enemyToDelete->release();

	}
	CCARRAY_FOREACH(bulletToDelete,bt)
	{
		Sprite* bullet =(Sprite*)bt;
		this->mutibullets->RemoveBullet(bullet);
	}
	bulletToDelete->removeAllObjects();
	bulletToDelete->release();
}
void MainScene::Mutibullet2AndEnemy()
{

	__Array* bulletToDelete=__Array::create();
	bulletToDelete->retain();
	Ref *bt,*et;

	CCARRAY_FOREACH(this->mutibullets->m_pAllBullet,bt)
	{
		Sprite *bullte =(Sprite*)bt;
		__Array* enemy2ToDelete = __Array::create();
		enemy2ToDelete->retain();

		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy2,et)
		{
			Enemy* enemy2 = (Enemy*)et;

			if (bullte->boundingBox().intersectsRect(enemy2->getBoundingBox()))
			{
				if (enemy2->getLife()>1)
				{
					enemy2->lostLife();
					bulletToDelete->addObject(bullte);
				}
				else if (enemy2->getLife()==1)
				{
					enemy2->lostLife();
					bulletToDelete->addObject(bullte);
					enemy2ToDelete->addObject(enemy2);
					playerScore += DIJI2_SCORE;
					this->controlLayer->updateScore(playerScore);
				}
				else;
				break;
			}
		}
		CCARRAY_FOREACH(enemy2ToDelete,et)
		{
			Enemy* enemy2=(Enemy*)et;
			this->enemyLayer->enemy2Blowup(enemy2);
		}
		enemy2ToDelete->release();

	}
	CCARRAY_FOREACH(bulletToDelete,bt)
	{
		Sprite* bullet =(Sprite*)bt;
		this->mutibullets->RemoveBullet(bullet);
	}
	bulletToDelete->removeAllObjects();
	bulletToDelete->release();
}
void MainScene::Mutibullet3AndEnemy()
{
	__Array* bulletToDelete=__Array::create();
	bulletToDelete->retain();
	Ref *bt,*et;
	CCARRAY_FOREACH(this->mutibullets->m_pAllBullet,bt)
	{
		Sprite *bullte =(Sprite*)bt;
		__Array* enemy2ToDelete = __Array::create();
		enemy2ToDelete->retain();
		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy3,et)
		{
			Enemy* enemy2 = (Enemy*)et;

			if (bullte->boundingBox().intersectsRect(enemy2->getBoundingBox()))
			{
				if (enemy2->getLife()>1)
				{
					enemy2->lostLife();
					bulletToDelete->addObject(bullte);
				}
				else if (enemy2->getLife()==1)
				{
					enemy2->lostLife();
					bulletToDelete->addObject(bullte);
					enemy2ToDelete->addObject(enemy2);

					playerScore += DIJI3_SCORE;
					this->controlLayer->updateScore(playerScore);
				}
				else;
				break;
			}
		}

		CCARRAY_FOREACH(enemy2ToDelete,et)
		{
			Enemy* enemy2=(Enemy*)et;
			this->enemyLayer->enemy3Blowup(enemy2);
		}
		enemy2ToDelete->release();

	}
	CCARRAY_FOREACH(bulletToDelete,bt)
	{
		Sprite* bullet =(Sprite*)bt;
		this->mutibullets->RemoveBullet(bullet);
	}
	bulletToDelete->removeAllObjects();
	bulletToDelete->release();
}
void MainScene::planeAndUFOCrash()
{
	Ref *ut;
	CCARRAY_FOREACH(this->ufoLayer->m_pAllMutiBullets,ut)
	{
		Sprite *mutiBulltes = (Sprite*)ut;
		if (mutiBulltes->boundingBox().intersectsRect(this->planeLayer->getBoundingBox()))
		{
			if (GameData::Inst()->isEffectOpen) {
				SimpleAudioEngine::getInstance()->playEffect("sound/get_bomb.mp3",false);
			}
			this->mutibullets->StartShoot(0.1f);
			Point point =mutiBulltes->getPosition();
			this->planeLayer->addJumpWord1(point);
			this->ufoLayer->RemoveMutiBullets(mutiBulltes);
			break;
		}

	}
}
void MainScene::planeAndUFOBoomCrash()
{
	Ref *ut;
	CCARRAY_FOREACH(this->ufoLayer->m_pAllBigBoomItem,ut)
	{
		Sprite *mutiBulltes = (Sprite*)ut;
		if (mutiBulltes->boundingBox().intersectsRect(this->planeLayer->getBoundingBox()))
		{
			if (GameData::Inst()->isEffectOpen) {
				SimpleAudioEngine::getInstance()->playEffect("sound/get_bomb.mp3",false);
			}

			Point point = mutiBulltes->getPosition();
			this->planeLayer->addJumpWord2(point);

			this->ufoLayer->RemoveBigBoomItem(mutiBulltes);
			bigBoomCount++;
			updateBigBoomItem(bigBoomCount);
			break;
		}

	}
}

void MainScene::updateBigBoomItem(int bigBoomCount)
{
	Sprite* normalBomb=Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("boom.png"));
	Sprite* pressedBomb=Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("boom.png"));
	if (bigBoomCount<0)
	{
		return;
	}
	else if (bigBoomCount==0)
	{
		if(this->getChildByTag(TAG_BIGBOOM_MENUITEM))
		{
			this->removeChildByTag(TAG_BIGBOOM_MENUITEM,true);

		}
		if (this->getChildByTag(TAG_BIGBOOMCOUNT_LABEL))
		{
			this->removeChildByTag(TAG_BIGBOOMCOUNT_LABEL,true);
		}
	}
	else if (bigBoomCount==1)
	{
		if (!this->getChildByTag(TAG_BIGBOOM_MENUITEM))
		{
			//auto pBigBoomItem = MenuItemImage::create("boom.png", "boom.png", CC_CALLBACK_1(MainScene::menuBigBoomCallback,this));
			auto pBigBoomItem = MenuItemSprite::create(normalBomb, pressedBomb,CC_CALLBACK_1(MainScene::menuBigBoomCallback,this));
			pBigBoomItem->setPosition(Point(normalBomb->getContentSize().width/2+10,normalBomb->getContentSize().height/2+10));
			menuBigBoom=CCMenu::create(pBigBoomItem,NULL);
			menuBigBoom->setPosition(Point::ZERO);
			this->addChild(menuBigBoom,0,TAG_BIGBOOM_MENUITEM);

		}
		if (this->getChildByTag(TAG_BIGBOOMCOUNT_LABEL))
		{
			this->removeChildByTag(TAG_BIGBOOMCOUNT_LABEL,true);
		}
	}
	else
	{
		if (!this->getChildByTag(TAG_BIGBOOM_MENUITEM))
		{
			//CCMenuItemImage* pBigBoomItem=CCMenuItemImage::create();
			//pBigBoomItem->initWithNormalSprite(normalBomb,pressedBomb,NULL,this,menu_selector(MainScene::menuBigBoomCallback));
			auto pBigBoomItem = MenuItemImage::create("boom.png", "boom.png", CC_CALLBACK_1(MainScene::menuBigBoomCallback,this));
			pBigBoomItem->setPosition(Point(normalBomb->getContentSize().width/2+10,normalBomb->getContentSize().height/2+10));
			menuBigBoom=CCMenu::create(pBigBoomItem,NULL);
			menuBigBoom->setPosition(Point::ZERO);
			this->addChild(menuBigBoom,0,TAG_BIGBOOM_MENUITEM);
		}
		if (this->getChildByTag(TAG_BIGBOOMCOUNT_LABEL))
		{
			this->removeChildByTag(TAG_BIGBOOMCOUNT_LABEL,true);
		}
		if (bigBoomCount>=0 && bigBoomCount<=MAX_BIGBOOM_COUNT)
		{
			//CCString* strScore=CCString::createWithFormat("X %d",bigBoomCount);
			char strScore[100];
			sprintf(strScore, "%d",bigBoomCount);
			//CCLOG("strScore = %c",strScore);
			//bigBoomCountItem=CCLabelBMFont::create(strScore->m_sString.c_str(),"font/font.fnt");
			//bigBoomCountItem=CCLabelTTF::create(strScore->m_sString.c_str(),"",12);
			// bigBoomCountItem=Label::createWithBMFont("font/font.fnt", strScore);
			bigBoomCountItem = Label::createWithSystemFont(strScore, "", 24);


			bigBoomCountItem->setColor(Color3B(143,146,147));
			bigBoomCountItem->setAnchorPoint(Point(0,0.5));

			bigBoomCountItem->setPosition(Point(normalBomb->getContentSize().width+15,normalBomb->getContentSize().height/2+5));
			this->addChild(bigBoomCountItem,0,TAG_BIGBOOMCOUNT_LABEL);
		}
	}
}
void MainScene::menuBigBoomCallback(Ref* pSender)
{
	if(bigBoomCount>0 && !Director::getInstance()->isPaused())
	{
		bigBoomCount--;
		Ref *et;
		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy1,et)
		{
			playerScore+=DIJI1_SCORE;
		}
		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy2,et)
		{
			playerScore+=DIJI2_SCORE;
		}
		CCARRAY_FOREACH(this->enemyLayer->m_pAllEnemy3,et)
		{
			playerScore+=DIJI3_SCORE;
		}
		this->controlLayer->updateScore(playerScore);
		this->enemyLayer->removeAllEnemy();
		updateBigBoomItem(bigBoomCount);
	}
}