package io.harness

import dagger.Binds
import dagger.Module
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import io.harness.services.FeatureFlagService
import io.harness.services.HarnessFeatureFlagService
import io.harness.services.MockFeatureFlagService
import io.harness.services.SwappableFeatureFlagService
import io.harness.settings.SettingsRepository
import io.harness.settings.SharedPreferencesSettingsRepository
import javax.inject.Qualifier

@Module
@InstallIn(SingletonComponent::class)
abstract class HarnessModule {

    @Binds
    abstract fun bindsSettingsRepository(sp: SharedPreferencesSettingsRepository): SettingsRepository

    @HarnessService
    @Binds
    abstract fun bindsHarnessFeatureFlagService(fs: HarnessFeatureFlagService): FeatureFlagService

    @MockService
    @Binds
    abstract fun bindsMockFeatureFlagService(fs: MockFeatureFlagService): FeatureFlagService

    @SwappableService
    @Binds
    abstract fun bindsSwappableFeatureFlagService(fs: SwappableFeatureFlagService): FeatureFlagService
}

@Qualifier
annotation class HarnessService

@Qualifier
annotation class MockService

@Qualifier
annotation class SwappableService