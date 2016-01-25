// Amazon Services Plugin
// Created by Patryk Stepniewski
// Copyright (c) 2014-2016 gameDNA studio. All Rights Reserved.

package com.amazon;

import java.util.Map;
import java.util.Set;
import java.util.ArrayList;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

import com.amazon.device.iap.PurchasingListener;
import com.amazon.device.iap.PurchasingService;
import com.amazon.device.iap.model.FulfillmentResult;
import com.amazon.device.iap.model.Product;
import com.amazon.device.iap.model.Receipt;
import com.amazon.device.iap.model.UserData;

import com.amazon.device.iap.model.ProductDataResponse;
import com.amazon.device.iap.model.PurchaseResponse;

import com.epicgames.ue4.GameActivity;

public class AmazonIapManager
{
    public static enum PurchaseStatus
    {
        PAID, FULFILLED, UNAVAILABLE, UNKNOWN
    }

    public static class PurchaseRecord
    {
        private PurchaseStatus status;
        private String receiptId;
        private String userId;

        public PurchaseStatus getStatus()
        {
            return status;
        }

        public void setStatus(final PurchaseStatus status)
        {
            this.status = status;
        }

        public String getReceiptId()
        {
            return receiptId;
        }

        public void setReceiptId(final String receiptId)
        {
            this.receiptId = receiptId;
        }

        public String getUserId()
        {
            return userId;
        }

        public void setUserId(final String userId)
        {
            this.userId = userId;
        }
    }

    private static final String TAG = "AmazonIAPManager";

    final private Context context;
    final private GameActivity mainActivity;
    private UserIapData userIapData;

    public AmazonIapManager(final GameActivity mainActivity)
    {
        this.mainActivity = mainActivity;
        this.context = mainActivity.getApplicationContext();
    }

    public void setAmazonUserId(final String newAmazonUserId, final String newAmazonMarketplace)
    {
        if (newAmazonUserId == null)
        {
            if (userIapData != null)
            {
                userIapData = null;
            }
        }
        else if (userIapData == null || !newAmazonUserId.equals(userIapData.getAmazonUserId()))
        {
            userIapData = new UserIapData(newAmazonUserId, newAmazonMarketplace);
        }
    }

    public void handleProductDataResponse(final ProductDataResponse response)
    {
        final Map<String, Product> productData = response.getProductData();
        final ProductDataResponse.RequestStatus status = response.getRequestStatus();
        ArrayList<String> titles = new ArrayList<String>();
        ArrayList<String> descriptions = new ArrayList<String>();
        ArrayList<String> prices = new ArrayList<String>();
        ArrayList<String> productIds = new ArrayList<String>();

        switch (status)
        {
            case SUCCESSFUL:
                for (Map.Entry<String, Product> entry : productData.entrySet())
                {
                    Product product = entry.getValue();
                    productIds.add(product.getSku());
                    titles.add(product.getTitle());
                    descriptions.add(product.getDescription());
                    prices.add(product.getPrice());
                    Log.i(TAG, "[AmazonIapManager] Retrieving In-App Item: " + product.getSku());
                }

                if (titles.size() > 0)
                {
                    Log.i(TAG, "[AmazonIapManager] - AmazonIapManager::handleProductDataResponse - Succeeded.");
                    nativeQueryCompleteAmazon(true, productIds.toArray(new String[productIds.size()]), titles.toArray(new String[titles.size()]), descriptions.toArray(new String[descriptions.size()]), prices.toArray(new String[prices.size()]));
                }
                else
                {
                    Log.i(TAG, "[AmazonIapManager] - AmazonIapManager::handleProductDataResponse - Failed. ");
                    nativeQueryCompleteAmazon(false, productIds.toArray(new String[productIds.size()]), null, null, null);
                }
                break;
            case FAILED:
                Log.i(TAG, "[AmazonIapManager] - AmazonIapManager::handleProductDataResponse - Failed. ");
                nativeQueryCompleteAmazon(false, productIds.toArray(new String[productIds.size()]), null, null, null);
                break;
            case NOT_SUPPORTED:
                Log.i(TAG, "[AmazonIapManager] - AmazonIapManager::handleProductDataResponse - Failed. ");
                nativeQueryCompleteAmazon(false, productIds.toArray(new String[productIds.size()]), null, null, null);
                break;
        }

        // Refresh user data and purchases updates
        PurchasingService.getUserData();
        PurchasingService.getPurchaseUpdates(false);
    }

    public void handlePurchaseResponse(final Receipt receipt, final UserData userData, boolean status)
    {
        if (status)
        {
            try
            {
                setAmazonUserId(userData.getUserId(), userData.getMarketplace());
                nativePurchaseCompleteAmazon(true, receipt.getSku(), receipt.getReceiptId());
                PurchasingService.notifyFulfillment(receipt.getReceiptId(), FulfillmentResult.FULFILLED);
            }
            catch (final Throwable e)
            {
                nativePurchaseCompleteAmazon(false, "", "");
                Log.e(TAG, "Failed to grant consumable purchase, with error " + e.getMessage());
            }
        }
        else
        {
            nativePurchaseCompleteAmazon(false, "", "");
        }
    }

    public UserIapData getUserIapData()
    {
        return this.userIapData;
    }

    public native void nativeQueryCompleteAmazon(boolean bSuccess, String[] productIDs, String[] titles, String[] descriptions, String[] prices);

    public native void nativePurchaseCompleteAmazon(boolean bSuccess, String ProductId, String ReceiptData);
}
