
# makefile for OpenWRT

include $(TOPDIR)/rules.mk

PKG_NAME:=bitsd-scripts
PKG_RELEASE:=1
PKG_VERSION:=1.1

include $(INCLUDE_DIR)/package.mk

define Package/bitsd-scripts
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=bitsd-scripts
  DEPENDS:=
endef

define Package/bitsd-scripts/description
	BITS System network connection scripts.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) * $(PKG_BUILD_DIR)/
endef

define Build/Compile
	echo "Nothing to be done"
endef

define Package/bitsd-scripts/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/crontabs
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usr/bin/*      $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/usr/lib/*      $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/etc/init.d/*   $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/etc/crontabs/* $(1)/etc/crontabs/
endef

$(eval $(call BuildPackage,bitsd-scripts))
