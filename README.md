# SurvivalGameDemo
First demo to learn UE4

> 编译前需在Visual Studio下包含目录$(SolutionDir)Source\$(ProjectName)



## 移动系统

// TODO: Crush



## 锁定系统

检测场景中所有的敌人，判断是否在锁定范围内，从角色向敌人发出一条射线是否被阻挡，用角色与敌人间的球体扫描优先锁定近处敌人，根据敌人与视野中线的夹角排序实现顺/逆时针切换锁定目标。

- 由于本项目摄像机处在角色正后方，可简单使用摄像机与角色位置差计算视线。

  ```cpp
  FVector CameraSight = GetActorLocation() - FollowCamera->GetComponentLocation();
  // (CameraSight.X, CameraSight.Y, 0.0f) 即可表示视野中线所在平面
  ```

  我们只需要求得敌人与视野中线在XY平面的相对角度，使用`FindLookAtRotation()`方法分别计算从摄像机看到角色与从摄像机看到敌人的视线相对于(1, 0, 0)的Rotator，对Yaw做差即为两者相对角度。

  ```cpp
  FRotator RotationFromPlayer = UKismetMathLibrary::FindLookAtRotation(CameraLocation, GetActorLocation());
  FRotator RotationFromEnemy = UKismetMathLibrary::FindLookAtRotation(CameraLocation, Enemy->GetActorLocation());
  float EnemyRelativePlayerYaw = RotationFromEnemy.Yaw - RotationFromPlayer.Yaw;
  ```

  当摄像机视野内存在敌人（正对），在角色左侧的敌人EnemyRelativePlayerYaw为负，右侧为正，越靠近视野中线绝对值越小，故EnemyRelativePlayerYaw绝对值最小的敌人为锁定目标。

  ![image-20220509215548372](https://mimir-archives-pic-1305889131.cos.ap-guangzhou.myqcloud.com/202205092155436.png)

  当摄像机视野内存无敌人（背对），有概率出现EnemyRelativePlayerYaw绝对值大于180°的情况，只需取该值相当于360°的另一边即可。由于锁定敌人时摄像机会自动转向敌人，故此处不再修正。

- 判断敌人与角色间有无遮挡使用`LineTraceSingleForObjects()`方法，从角色向敌人间发射一条射线。

  ```cpp
  FVector StartLocation = GetActorLocation();	
  FVector EndLocation = OtherActor->GetActorLocation();
  TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypes;	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
  ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
  bool bTraceComplex = false;
  const TArray<AActor*> ActorsToIgnore{ this };
  EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::None;
  FHitResult OutHit;
  bool bIgnoreSelf = true;
  
  bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, StartLocation, EndLocation, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
  ```

  若敌人间有遮挡关系，逻辑上应该锁定就近敌人。使用`SphereTraceSingleForObjects()`方法对相对于视野中线最近的敌人做球体追踪检测，路径上有其他敌人则更换锁定对象。

  ![image-20220509220445866](https://mimir-archives-pic-1305889131.cos.ap-guangzhou.myqcloud.com/202205092204070.png)

- 在锁定期间保持对摄像机朝向的更新，若角色翻滚则需要在翻滚期间使角色面向敌人，实现八向翻滚。

  ```cpp
  void AMainPlayer::UpdateLockingCameraRotation(float DeltaTime)
  {
  	const FRotator CurrentRotation = Controller->GetControlRotation();
  	float InterpSpeed = 10.0f;
  	const FRotator RotationFromCameraToEnemy = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), CurrentLockingEnemy->GetActorLocation());
  	
  	// Pitch和Yaw更新，Roll保持不变
  	const FRotator TargetRotation = FRotator(RotationFromCameraToEnemy.Pitch, RotationFromCameraToEnemy.Yaw, CurrentRotation.Roll);
  
  	// 插值平滑过渡
  	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
  	Controller->SetControlRotation(NewRotation);
  }
  
  void AMainPlayer::UpdateLockingActorRotation(float DeltaTime)
  {
  	if (bRolling)
  	{
  		const FRotator CurrentRotation = GetActorRotation();
  		float InterpSpeed = 5.0f;
  		const FRotator RotationFromPlayerToEnemy = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentLockingEnemy->GetActorLocation());
  
  		// Yaw更新
  		const FRotator TargetRotation = FRotator(CurrentRotation.Pitch, RotationFromPlayerToEnemy.Yaw, CurrentRotation.Roll);
  
  		// 插值平滑过渡
  		const FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
  		SetActorRotation(NewRotation);
  	}
  }
  ```

  结束锁定时将摄像机的Pitch设为0，使用插值平滑过渡，对于锁定大体型敌人很友好。

  ![image](https://github.com/Purlemon/SurvivalGameDemo/tree/main/Image/GIF2022-5-9-22-34-47.gif)

- 在遍历寻找锁定敌人时以敌人为key、与视野中线相对Yaw为value将可锁定的敌人全部存在TMap中（CanLockedEnemies），切换锁定只需要对key排序，根据当前锁定敌人在key数组的索引切换上一个/下一个索引。

  ```cpp
  void AMainPlayer::SwitchLocked(bool bIsLeft)
  {
  	if (CurrentLockingEnemy && bLocking)
  	{
  		ABaseEnemy* OldLockingEnemy = CurrentLockingEnemy;
  		bool bHasEnemies = FindAndUpdateCanLockedEnemies();
  		if (bHasEnemies)
  		{
  			// 将全部敌人按照顺时针排序（视野中线从左到右）
  			CanLockedEnemies.ValueSort([](float A, float B) {
  				return A < B;
  				});
  
  			TArray<ABaseEnemy*> Enemies;
  			CanLockedEnemies.GenerateKeyArray(Enemies);
  			// 得到当前锁定敌人在全部敌人中的索引
  			int32 OldLockedEnemyIndex = Enemies.Find(OldLockingEnemy);
  
  			if (bIsLeft)
  			{
  				if (OldLockedEnemyIndex == 0)
  				{
  					CurrentLockingEnemy = Enemies[Enemies.Num() - 1];
  				}
  				else
  				{
  					CurrentLockingEnemy = Enemies[OldLockedEnemyIndex - 1];
  				}
  			} 
  			else
  			{
  				if (OldLockedEnemyIndex == Enemies.Num() - 1)
  				{
  					CurrentLockingEnemy = Enemies[0];
  				}
  				else
  				{
  					CurrentLockingEnemy = Enemies[OldLockedEnemyIndex + 1];
  				}
  			}
  			// 设置锁定标记可见性
  			OldLockingEnemy->LockedMarkMesh->SetVisibility(false);
  			CurrentLockingEnemy->LockedMarkMesh->SetVisibility(true);
  		}
  	}
  }
  ```

  ![image](https://github.com/Purlemon/SurvivalGameDemo/tree/main/Image/GIF2022-5-9-22-40-03.gif)





## 武器
