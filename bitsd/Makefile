
# makefile for OpenWRT

include $(TOPDIR)/rules.mk

PKG_NAME:=bitsd
PKG_RELEASE:=1
PKG_VERSION:=1.03

include $(INCLUDE_DIR)/package.mk

define Package/bitsd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=bitsd
  DEPENDS:=+libstdcpp +boost-system
endef

define Package/bitsd/description
	BITS System client daemon.
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

define Package/bitsd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bitsd $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bitsd.conf $(1)/etc/config/
endef

$(eval $(call BuildPackage,bitsd))
