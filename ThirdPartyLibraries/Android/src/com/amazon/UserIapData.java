// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

package com.amazon;

public class UserIapData
{
    private final String amazonUserId;
    private final String amazonMarketplace;

    public String getAmazonUserId()
    {
        return amazonUserId;
    }

    public String getAmazonMarketplace()
    {
        return amazonMarketplace;
    }

    public UserIapData(final String amazonUserId, final String amazonMarketplace)
    {
        this.amazonUserId = amazonUserId;
        this.amazonMarketplace = amazonMarketplace;
    }
}
