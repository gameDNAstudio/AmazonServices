// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

package com.amazon;

import android.util.Log;

import com.amazon.device.iap.PurchasingListener;
import com.amazon.device.iap.PurchasingService;
import com.amazon.device.iap.model.ProductDataResponse;
import com.amazon.device.iap.model.PurchaseResponse;
import com.amazon.device.iap.model.PurchaseUpdatesResponse;
import com.amazon.device.iap.model.Receipt;
import com.amazon.device.iap.model.UserDataResponse;

import java.util.HashSet;
import java.util.Set;

public class AmazonPurchasingListener implements PurchasingListener
{
    private static final String TAG = "AmazonIAPConsumablesApp";
    private final AmazonIapManager iapManager;

    public AmazonPurchasingListener(final AmazonIapManager iapManager)
    {
        this.iapManager = iapManager;
    }

    @Override
    public void onUserDataResponse(final UserDataResponse response)
    {
        final UserDataResponse.RequestStatus status = response.getRequestStatus();
        switch (status)
        {
            case SUCCESSFUL:
                iapManager.setAmazonUserId(response.getUserData().getUserId(), response.getUserData().getMarketplace());
                break;
            case FAILED:
            case NOT_SUPPORTED:
                iapManager.setAmazonUserId(null, null);
                break;
        }
    }

    @Override
    public void onProductDataResponse(final ProductDataResponse response)
    {
        iapManager.handleProductDataResponse(response);
    }

    @Override
    public void onPurchaseUpdatesResponse(final PurchaseUpdatesResponse response)
    {
        for (final Receipt receipt : response.getReceipts())
        {
            try
            {
                iapManager.handlePurchaseResponse(receipt, response.getUserData(), true);
            }
            catch (Exception e)
            {
            }
        }
        if (response.hasMore())
        {
            PurchasingService.getPurchaseUpdates(false);
        }
    }

    @Override
    public void onPurchaseResponse(final PurchaseResponse response)
    {
        try
        {
            iapManager.handlePurchaseResponse(response.getReceipt(), response.getUserData(), response.getRequestStatus() == PurchaseResponse.RequestStatus.SUCCESSFUL);
        }
        catch (Exception e)
        {
            iapManager.handlePurchaseResponse(null, response.getUserData(), false);
        }
    }
}
