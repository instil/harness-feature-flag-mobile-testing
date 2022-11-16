package io.harness

import dagger.Binds
import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import io.harness.services.FeatureFlagService
import io.harness.services.HarnessFeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SharedPreferencesSettingsRepository

@Module
@InstallIn(SingletonComponent::class)
abstract class HarnessModule {

    @Binds
    abstract fun bindsSettingsRepository(sp: SharedPreferencesSettingsRepository): SettingsRepository

    @Binds
    abstract fun bindsFeatureFlagService(fs: HarnessFeatureFlagService): FeatureFlagService

}