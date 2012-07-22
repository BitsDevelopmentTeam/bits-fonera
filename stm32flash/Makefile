
# makefile for OpenWRT

include $(TOPDIR)/rules.mk

PKG_NAME:=stm32flash
PKG_RELEASE:=0
PKG_VERSION:=svn61

include $(INCLUDE_DIR)/package.mk

define Package/stm32flash
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=stm32flash
  DEPENDS:=
endef

define Package/stm32flash/description
	Program stm32 microcontrollers via serial port.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)   \
		CFLAGS="$(TARGET_CFLAGS)"                      \
		CXXFLAGS="$(TARGET_CFLAGS) $(TARGET_CXXFLAGS) -I$(STAGING_DIR)/usr/include" \
		LFLAGS="$(TARGET_CFLAGS) $(TARGET_LDFLAGS)" 
endef

define Package/stm32flash/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/stm32flash $(1)/usr/bin/
endef

$(eval $(call BuildPackage,stm32flash))
