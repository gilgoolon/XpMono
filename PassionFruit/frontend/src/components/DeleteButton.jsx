import React from 'react';
import { IconButton } from '@mui/material';
import DeleteIcon from '@mui/icons-material/Delete';

export function DeleteButton({ onDelete }) {
    return (
        <IconButton
            color="error"
            onClick={(e) => {
                e.stopPropagation();
                onDelete();
            }}
            aria-label="delete"
        >
            <DeleteIcon />
        </IconButton>
    );
}
